#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::quit(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	std::vector<std::string> channelList = client_iter->second->getChannelList();
	std::vector<std::string>::iterator channel_iter = channelList.begin();
	for (; channel_iter != channelList.end(); channel_iter++)
	{
		Channel *channel = _server.findChannel(*channel_iter);
		channel->removeClientFdList(fd);
		channel->removeOperatorFd(fd);
		if (channel)
		{
			msgToAllChannel(fd, channel->getChannelName(), "QUIT", channelMessage(1, command_vec));
		}
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
	client_iter->second->setRegist(false);
	clients.erase(fd);
	close(fd);
}
