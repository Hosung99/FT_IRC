#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::ping(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);

	if (command_vec.size() < 2)
	{
		iter->second->appendClientRecvBuf("461 :");
		iter->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		iter->second->appendClientRecvBuf("/PING <token>\r\n");
		return;
	}
	iter->second->appendClientRecvBuf("PONG " + command_vec[1] + "\r\n");
}
