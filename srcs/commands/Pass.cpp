#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"


void Command::pass(int fd, std::vector<std::string> command_vec)
{
	/* PASS <password> */
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator iter = clients.find(fd);
	std::string password = _server.getPassword();
	if (iter->second.getPassRegist())
	{
		err_alreadyregistred_462(iter->second);
		return;
	}
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(iter->second);
		return;
	}
	if (strcmp(command_vec[1].c_str(), password.c_str()) != 0)
	{
		err_passwdmismatch_464(iter->second);
		send(fd, iter->second.getClientRecvBuf().c_str(), iter->second.getClientRecvBuf().length(), 0);
		iter->second.clearClient();
		clients.erase(fd);
		close(fd);
		return;
	}
	iter->second.setPassRegist(true);
}