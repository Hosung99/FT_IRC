#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::ping(int fd, std::vector<std::string> command_vec)
{
	/* PING <token> */
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator iter = clients.find(fd);

	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(iter->second);
		iter->second.appendClientRecvBuf("/PING <token>\r\n");
		return;
	}
	iter->second.appendClientRecvBuf("PONG " + command_vec[1] + "\r\n");
}
