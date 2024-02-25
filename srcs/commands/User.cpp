#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::user(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (iter->second->getUserRegist())
	{
		iter->second->appendClientRecvBuf("462 : ");
		iter->second->appendClientRecvBuf(ERR_ALREADYREGIST);
		return;
	}
	if (!iter->second->getPassRegist())
	{
		iter->second->appendClientRecvBuf("451 :");
		iter->second->appendClientRecvBuf(ERR_NOTREGISTERED);
		iter->second->appendClientRecvBuf("\r\n");
		send(fd, iter->second->getClientRecvBuf().c_str(), iter->second->getClientRecvBuf().length(), 0);
		clients.erase(fd);
		close(fd);
		return;
	}
	if (command_vec.size() < 5 || !checkRealname(command_vec[4]))
	{
		iter->second->appendClientRecvBuf("461 USER :");
		iter->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		iter->second->appendClientRecvBuf("/USER <username> <hostname> <servername> <:realname>\r\n");
		return;
	}
	std::string realname;
	for (size_t i = 4; i < command_vec.size(); i++)
	{
		realname += command_vec[i];
		if (i != command_vec.size() - 1)
			realname += " ";
	}
	iter->second->setUser(command_vec[1], command_vec[2], command_vec[3], realname);
	iter->second->setUserRegist(true);
}

bool Command::checkRealname(std::string realname)
{
	if (realname.length() == 0)
		return (false);
	if (realname[0] != ':')
		return (false);
	return (true);
}