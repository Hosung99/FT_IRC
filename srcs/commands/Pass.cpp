#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"


void Command::pass(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	std::string password = _server.getPassword();
	if (iter->second->getPassRegist())
	{
		iter->second->appendClientRecvBuf("462 : ");
		iter->second->appendClientRecvBuf(ERR_ALREADYREGIST);
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->appendClientRecvBuf("461 PASS: ");
		iter->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	if (strcmp(command_vec[1].c_str(), password.c_str()) != 0)
	{
		iter->second->appendClientRecvBuf("464 :");
		iter->second->appendClientRecvBuf(ERR_PASSWDMISMATCH);
		send(fd, iter->second->getClientRecvBuf().c_str(), iter->second->getClientRecvBuf().length(), 0);
		iter->second->clearClient();
		clients.erase(fd);
		// close(fd);
		return;
	}
	iter->second->setPassRegist(true);
}