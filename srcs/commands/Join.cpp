#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::join(int fd, std::vector<std::string> command_vec)
{
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(_server.getClients().find(fd)->second);
		return;
	}
	std::vector<std::string> joinChannel = split(command_vec[1], ',');
	std::vector<std::string>::iterator iter = joinChannel.begin();
	std::vector<std::string> joinKey;
	std::vector<std::string>::iterator keyIter;
	if (command_vec.size() > 2)
	{
		joinKey = split(command_vec[2], ',');
		keyIter = joinKey.begin();
	}
	std::map<int, Client *> clients = _server.getClients();
	Client *client = clients.find(fd)->second;
	while (iter != joinChannel.end())
	{
		if ((*iter)[0] != '#' && (*iter)[0] != '&')
		{
			err_nosuchchannel_403(client, *iter);
			iter++;
			if (command_vec.size() > 2 || keyIter != joinKey.end())
				keyIter++;
			continue;
		}
		std::map<std::string, Channel *> &channelList = _server.getChannelList();
		std::map<std::string, Channel *>::iterator channelIt = channelList.find(*iter);
		if (channelIt != channelList.end()) // 채널이 있다면
		{
			// 해당 클라이언트를 채널에 넣어준다.
			Channel *channel = channelIt->second;
			if (channel->checkClientInChannel(fd))
			{
				iter++;
				if (command_vec.size() > 2 || keyIter != joinKey.end())
					keyIter++;
				continue;
			}
			if (channel->checkMode(INVITE)) // INVITE 모드가 켜져있다면
			{
				if (!channel->checkInvite(fd))
				{
					err_inviteonlychan_473(client, *iter);
					iter++;
					if (command_vec.size() > 2 || keyIter != joinKey.end())
						keyIter++;
					continue;
				}
			}
			if (channel->checkMode(KEY)) // KEY 모드가 켜져있다면
			{
				if (command_vec.size() <= 2 || keyIter == joinKey.end() || !channel->checkKey(*keyIter)) // 인자에 키가 없거나 키가 틀리다면
				{
					err_badchannelkey_475(client, *iter);
					iter++;
					if (command_vec.size() > 2 || keyIter != joinKey.end())
						keyIter++;
					continue;
				}
			}
			if (channel->checkMode(LIMIT)) // LIMIT 모드가 켜져있다면
			{
				if (channel->getClientFdList().size() >= channel->getLimit())
				{
					err_channelisfull_471(client, *iter);
					iter++;
					if (command_vec.size() > 2 || keyIter != joinKey.end())
						keyIter++;
					continue;
				}
			}
			std::string channelName = (*channelIt).second->getChannelName();
			(*channelIt).second->appendClientFdList(fd);
			client->appendChannelList(channelName);		  // 클라이언트에 채널을 추가 해준다.
			msgToAllChannel(fd, channelName, "JOIN", ""); // 채널에 JOIN 메시지를 보내준다.
			topicMsg(fd, channelName);
		}
		else // 채널이 없다면
		{
			_server.appendNewChannel(*iter, fd);				// 서버에 채널을 추가 해준다.
			_server.findChannel(*iter)->appendClientFdList(-1);
			_server.findChannel(*iter)->appendClientFdList(fd); // 해당 클라이언트를 채널에 넣어준다.
			client->appendChannelList(*iter);
			msgToAllChannel(fd, *iter, "JOIN", "");
			_server.findChannel(*iter)->addOperatorFd(fd);
		}
		nameListMsg(fd, *iter);
		msgToAllChannel(-1, *iter, "PRIVMSG", _server.findChannel(*iter)->getBot()->introduce());
		iter++;
		if (command_vec.size() > 2 || keyIter != joinKey.end())
			keyIter++;
	}
}

void Command::topicMsg(int fd, std::string channelName)
{
	std::map<std::string, Channel *> channelList = _server.getChannelList();
	Channel *channel = channelList.find(channelName)->second;
	std::string topic = channel->getTopic();
	if (topic.length() == 0)
	{
		return;
	}
	topic = topic.substr(1, topic.length() - 1);
	Client *client = _server.getClients().find(fd)->second;
	client->appendClientRecvBuf("332 " + client->getNickname() + " " + channelName + " :" + topic + "\r\n");
}
