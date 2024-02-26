#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::nick(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (!iter->second->getPassRegist())
	{
		iter->second->appendClientRecvBuf("451 :");
		iter->second->appendClientRecvBuf(ERR_NOTREGISTERED);
		send(fd, iter->second->getClientRecvBuf().c_str(), iter->second->getClientRecvBuf().length(), 0);
		clients.erase(fd);
		close(fd);
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->appendClientRecvBuf("431 :");
		iter->second->appendClientRecvBuf(ERR_NONICKNAMEGIVEN);
		return;
	}
	if (command_vec[1] == "_")
	{
		while(1)
		{
			Client *client = _server.findClient(command_vec[1]);
			if (client != NULL)
				command_vec[1] = client->getNickname() + "_";
			else
				break ;
		}
	}
	if (!checkNicknameValidate(command_vec[1]))
	{
		iter->second->appendClientRecvBuf("432 :");
		iter->second->appendClientRecvBuf(ERR_ERRONEUSNICKNAME);
		iter->second->appendClientRecvBuf("/NICK <nickname> First Letter is not digit and length is under 10.\r\n");
		return;
	}
	if (!checkNicknameDuplicate(command_vec[1], _server.getClients()))
	{
		iter->second->appendClientRecvBuf("433 :");
		iter->second->appendClientRecvBuf(ERR_NICKNAMEINUSE);
		return;
	}
	std::string old_nickname = iter->second->getNickname();
	if (old_nickname == "Client")
		old_nickname = iter->second->getNickname();
	std::vector<std::string> channelList = iter->second->getChannelList();
	std::vector<std::string>::iterator channel_iter = channelList.begin();
	for (; channel_iter != channelList.end(); channel_iter++)
	{
		Channel *channel = _server.findChannel(*channel_iter);
		if (channel)
			msgToAllChannel(fd, channel->getChannelName(), "NICK", old_nickname + " " + command_vec[1]);
	}
	iter->second->setNickname(command_vec[1]);
	iter->second->appendClientRecvBuf(":" + old_nickname + " NICK " + iter->second->getNickname() + "\r\n");
	iter->second->setNickRegist(true);
}


bool Command::checkNicknameValidate(std::string nickname)
{
	if (nickname.length() == 0 || nickname.length() > 9)
		return (false);
	if (nickname[0] >= '0' && nickname[0] <= '9')
		return (false);
	for (size_t i = 1; i < nickname.length(); i++)
	{
		if (!isalnum(nickname[i]) && !isspecial(nickname[i]))
			return (false);
	}
	return (true);
}

bool Command::checkNicknameDuplicate(std::string nickname, std::map<int, Client *> Clients)
{
	std::map<int, Client *>::iterator iter = Clients.begin();
	for (; iter != Clients.end(); iter++)
	{
		if (iter->second->getNickname() == nickname)
		{
			return (false);
		}
	}
	return (true);
}