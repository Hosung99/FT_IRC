#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::kick(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 3)
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
	Channel *channel = _server.findChannel(*vec_iter);
	if (channel && !channel->checkOperator(fd))
	{
		client_iter->second->appendClientRecvBuf("482 " + *vec_iter + " :" + ERR_CHANOPRIVSNEEDED);
		return;
	}
	for (; vec_iter != vec.end(); vec_iter++)
	{
		Channel *channel = _server.findChannel(*vec_iter);
		if (channel == NULL)
		{
			client_iter->second->appendClientRecvBuf("403 " + *vec_iter + " :" + ERR_NOSUCHCHANNEL);
		}
		else
		{
			Client *target = _server.findClient(command_vec[2]);
			if (target == NULL)
			{
				client_iter->second->appendClientRecvBuf("401 " + command_vec[2] + " :" + ERR_NOSUCHNICK);
			}
			if (target->getNickname() == client_iter->second->getNickname())
			{
				return;
			}
			else
			{
				if (!channel->checkClientInChannel(target->getClientFd()))
				{
					client_iter->second->appendClientRecvBuf("441 " + command_vec[2] + " " + *vec_iter + " :" + ERR_USERNOTINCHANNEL);
				}
				else
				{
					std::string message = command_vec[2];
					if (command_vec.size() > 3)
						message += " " + command_vec[3];
					msgToAllChannel(fd, *vec_iter, "KICK", message);
					channel->removeClientFdList(target->getClientFd());
					target->removeChannel(*vec_iter);
				}
			}
		}
	}
}
