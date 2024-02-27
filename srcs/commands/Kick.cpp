#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::kick(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 3)
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
	Channel *channel = _server.findChannel(*vec_iter);
	if (channel && !channel->checkOperator(fd))
	{
		err_chanoprivsneeded_482(client_iter->second, *vec_iter);
		return;
	}
	for (; vec_iter != vec.end(); vec_iter++)
	{
		Channel *channel = _server.findChannel(*vec_iter);
		if (channel == NULL)
		{
			err_nosuchchannel_403(client_iter->second, *vec_iter);
		}
		else
		{
			std::map<int, Client>::iterator target = _server.findClient(command_vec[2]);
			if (target == _server.getClients().end())
			{
				err_nosuchnick_401(client_iter->second, command_vec[2]);
				return;
			}
			if (target->second.getClientFd() == -1)
			{
				err_nosuchnick_401(client_iter->second, command_vec[2]);
				return;
			}
			if (target->second.getNickname() == client_iter->second.getNickname())
			{
				return;
			}
			else
			{
				if (!channel->checkClientInChannel(target->second.getClientFd()))
				{
					err_usernotinchannel_441(client_iter->second, command_vec[2], *vec_iter);
				}
				else
				{
					std::string message = command_vec[2];
					if (command_vec.size() > 3)
						message += " " + command_vec[3];
					msgToAllChannel(fd, *vec_iter, "KICK", message);
					channel->removeClientFdList(target->second.getClientFd());
					target->second.removeChannel(*vec_iter);
				}
			}
		}
	}
}
