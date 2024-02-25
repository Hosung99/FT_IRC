#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::part(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
	{
		client_iter->second->appendClientRecvBuf("461 :");
		client_iter->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	std::istringstream iss(command_vec[1]);
	std::string buffer;
	std::vector<std::string> vec;
	while (getline(iss, buffer, ','))
		vec.push_back(buffer);
	std::vector<std::string>::iterator vec_iter = vec.begin();
	for (; vec_iter != vec.end(); vec_iter++)
	{
		std::vector<std::string>::iterator channelList = client_iter->second->findChannel(*vec_iter);
		if (channelList != client_iter->second->getChannelList().end())
		{
			Channel *channel = _server.findChannel(*channelList);
			msgToAllChannel(fd, *vec_iter, "PART", channelMessage(2, command_vec));
			channel->removeClientFdList(fd);
			channel->removeOperatorFd(fd);
			client_iter->second->removeChannel(*channelList);
			if (channel->getClientFdList().size() == 1)
			{
				_server.removeChannel(channel->getChannelName());
				delete channel;
			}
			else
			{
				std::vector<int> fdList = channel->getClientFdList();
				std::vector<int>::iterator fd_iter = fdList.begin();
				fd_iter++;
				channel->addOperatorFd(*fd_iter);
			}
		}
		else
		{
			if (_server.findChannel(*vec_iter))
			{
				client_iter->second->appendClientRecvBuf("442 " + *vec_iter + " :" + ERR_NOTONCHANNEL);
			}
			else
			{
				client_iter->second->appendClientRecvBuf("403 " + *vec_iter + " :" + ERR_NOSUCHCHANNEL);
			}
		}
	}
}
