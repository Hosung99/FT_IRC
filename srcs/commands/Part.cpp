#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::part(int fd, std::vector<std::string> command_vec)
{
	/* PART <channel> <nickname> (<reasons, ...>) */
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(client_iter->second);
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
		std::vector<std::string>::iterator channelList = client_iter->second.findChannel(*vec_iter);
		if (channelList != client_iter->second.getChannelList().end())
		{
			Channel *channel = _server.findChannel(*channelList);
			msgToAllChannel(fd, *vec_iter, "PART", channelMessage(2, command_vec));
			channel->removeClientFdList(fd);
			channel->removeOperatorFd(fd);
			client_iter->second.removeChannel(*channelList);
			if (channel->getClientFdList().size() == 1)		// if last-client in channel: remove channel
			{
				_server.removeChannel(channel->getChannelName());
				delete channel;
			}
		}
		else	// error
		{
			if (_server.findChannel(*vec_iter))
			{
				err_notonchannel_442(client_iter->second, *vec_iter);
			}
			else
			{
				err_nosuchchannel_403(client_iter->second, *vec_iter);
			}
		}
	}
}
