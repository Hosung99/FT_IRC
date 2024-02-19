#include "Command.hpp"

Command::Command(Server &server) : _server(server) {}
Command::~Command() {}

static bool isspecial(char c)
{
	if (c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|')
		return (true);
	return (false);
}

void Command::run(int fd)
{
	std::istringstream iss(_server.getMessage(fd));
	std::string buffer;
	std::map<int, Client *>::iterator iter;
	std::vector<std::string> command_vec;
	std::map<int, Client *> clients = _server.getClients();
	iter = clients.find(fd);
	while (getline(iss, buffer, ' '))
	{
		std::size_t endPos = buffer.find_last_not_of("\r\n");
		command_vec.push_back(buffer.substr(0, endPos + 1));
	}
	if (!iter->second->get_is_regist())
	{
		if (command_vec[0] == "PASS")
		{
			pass(fd, command_vec);
		}
		else if (command_vec[0] == "NICK")
		{
			nick(fd, command_vec);
		}
		else if (command_vec[0] == "USER")
		{
			user(fd, command_vec);
		}
		else
		{
			iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
			iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
			clients.erase(fd);
			close(fd);
			delete iter->second;
		}
		if (iter->second && iter->second->get_is_regist())
		{
			iter->second->append_client_recv_buf("001 :Welcome to the Internet Relay Network, " + iter->second->get_nickname() + "\r\n");
		}
	}
	else
	{
		if (command_vec[0] == "PING")
			ping(fd, command_vec);
		else if (command_vec[0] == "USER")
			user(fd, command_vec);
		else if (command_vec[0] == "NICK")
			nick(fd, command_vec);
		else if (command_vec[0] == "PASS")
			pass(fd, command_vec);
		// else
		// {
		// 	iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		// 	iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		// 	clients.erase(fd);
		// 	close(fd);
		// 	delete iter->second;
		// }
	}
}

void Command::pass(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	std::string password = _server.getPassword();
	if (iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf("462 :");
		iter->second->append_client_recv_buf(ERR_ALREADYREGIST);
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->append_client_recv_buf("461 PASS: ");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		return;
	}
	if (strcmp(command_vec[1].c_str(), password.c_str()) != 0)
	{
		iter->second->append_client_recv_buf("464 :");
		iter->second->append_client_recv_buf(ERR_PASSWDMISMATCH);
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	iter->second->set_pass_regist(true);
}

void Command::nick(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (!iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf("451 :");
		iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->append_client_recv_buf("431 :");
		iter->second->append_client_recv_buf(ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!checkNicknameValidate(command_vec[1]))
	{
		iter->second->append_client_recv_buf("432 :");
		iter->second->append_client_recv_buf(ERR_ERRONEUSNICKNAME);
		iter->second->append_client_recv_buf("/NICK <nickname> First Letter is not digit and length is under 10.\r\n");
		return;
	}
	if (!checkNicknameDuplicate(command_vec[1], _server.getClients()))
	{
		iter->second->append_client_recv_buf("433 :");
		iter->second->append_client_recv_buf(ERR_NICKNAMEINUSE);
		return;
	}
	std::string old_nickname = iter->second->get_nickname();
	iter->second->set_nickname(command_vec[1]);
	if (old_nickname == "Client")
		old_nickname = iter->second->get_nickname();
	iter->second->append_client_recv_buf(":" + old_nickname + " NICK " + iter->second->get_nickname() + "\r\n");
	iter->second->set_nick_regist(true);
	// + 해당 클라이언트가 속한 모든 채널을 돌며 닉네임을 바꾸었다고 알려준다.
}

void Command::user(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (iter->second->get_user_regist())
	{
		iter->second->append_client_recv_buf("462 :");
		iter->second->append_client_recv_buf(ERR_ALREADYREGIST);
		return;
	}
	if (!iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf("451 :");
		iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		iter->second->append_client_recv_buf("\r\n");
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	if (command_vec.size() < 5 || !checkRealname(command_vec[4]))
	{
		iter->second->append_client_recv_buf("461 USER :");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		iter->second->append_client_recv_buf("/USER <username> <hostname> <servername> <:realname>\r\n");
		return;
	}
	std::string realname;
	for(size_t i = 4; i < command_vec.size(); i++)
	{
		realname += command_vec[i];
		if (i != command_vec.size() - 1)
			realname += " ";
	}
	iter->second->set_user(command_vec[1], command_vec[2], command_vec[3], realname);
	iter->second->set_user_regist(true);
}

void Command::ping(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);

	if (command_vec.size() < 2)
	{
		iter->second->append_client_recv_buf("461 :");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		iter->second->append_client_recv_buf("/PING <token>\r\n");
		return;
	}
	iter->second->append_client_recv_buf("PONG " + command_vec[1] + "\r\n");
}

bool Command::checkNicknameValidate(std::string nickname)
{
	if (nickname.length() == 0 || nickname.length() > 9)
		return (false);
	if (nickname[0] >= '0' && nickname[0] <= '9')
		return (false);
	for (size_t i = 1; i < nickname.length(); i++)
	{
		if (!isalnum(nickname[i]) && isspecial(nickname[i]))
			return (false);
	}
	return (true);
}

bool Command::checkNicknameDuplicate(std::string nickname, std::map<int, Client *> Clients)
{
	std::map<int, Client *>::iterator iter = Clients.begin();
	for (; iter != Clients.end(); iter++)
	{
		if (iter->second->get_nickname() == nickname)
			return (false);
	}
	return (true);
}

bool Command::checkRealname(std::string realname)
{
	if (realname.length() == 0)
		return (false);
	if (realname[0] != ':')
		return (false);
	return (true);
}