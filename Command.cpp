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
	std::istringstream iss(_server.get_message(fd));
	std::string buffer;
	std::map<int, Client *>::iterator iter;
	std::vector<std::string> command_vec;
	std::map<int, Client *> clients = _server.get_clients();
	iter = clients.find(fd);
	while (getline(iss, buffer, ' '))
	{
		command_vec.push_back(buffer);
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
	}
	else
	{
		// regist 된 client에 대한 명령어 처리
	}
}

void Command::pass(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.get_clients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	std::string password = _server.get_password();
	if (command_vec.size() < 2) // rfc문서상으론 multple pass commands가 가능하다는데, 그게 PASS command가 여러개가 들어오는건지, PASS command에 여러개의 password가 들어오는건지 모르겠음
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " ");
		iter->second->append_client_recv_buf("PASS: ");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		return;
	}
	if (iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_ALREADYREGIST);
		return;
	}
	if (strcmp(command_vec[1].substr(0, command_vec[1].length() - 2).c_str(), password.c_str()) != 0)
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
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
	std::map<int, Client *> clients = _server.get_clients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (!iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		clients.erase(fd);
		close(fd);
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!check_nickname_validate(command_vec[1]))
	{
		iter->second->append_client_recv_buf(command_vec[1] + " :");
		iter->second->append_client_recv_buf(ERR_ERRONEUSNICKNAME);
		iter->second->append_client_recv_buf("/NICK <nickname> First Letter is not digit and length is under 10.\r\n");
		return;
	}
	if (!check_nickname_duplicate(command_vec[1], _server.get_clients()))
	{
		iter->second->append_client_recv_buf(command_vec[1] + " :");
		iter->second->append_client_recv_buf(ERR_NICKNAMEINUSE);
		return;
	}
	if (!iter->second->get_nick_regist())
	{
		iter->second->set_nickname(command_vec[1]);
		iter->second->append_client_recv_buf(":" + iter->second->get_nickname() + "NICK" + iter->second->get_nickname() + "\r\n");
	}
	else
	{
		// 등록 되어있다면, 해당 클라이언트 fd의 닉네임을 바꾸겠단 뜻이다.
		// 해당 클라이언트가 속한 모든 채널을 돌며 닉네임을 바꾸었다고 알려준다.
	}
}

void Command::user(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.get_clients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (!iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		iter->second->append_client_recv_buf("\r\n");
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	if (command_vec.size() < 5 || !check_realname(command_vec[4]))
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " USER :");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		iter->second->append_client_recv_buf("/USER <username> <hostname> <servername> <:realname>\r\n");
		return;
	}
	if (iter->second->get_user_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_ALREADYREGIST);
		return;
	}
	iter->second->set_user(command_vec[1], command_vec[2], command_vec[3], command_vec[4]);
	iter->second->set_user_regist(true);
}

bool Command::check_nickname_validate(std::string nickname)
{
	if (nickname.length() == 0 || nickname.length() > 9)
		return (false);
	if (nickname[0] >= '0' && nickname[0] <= '9')
		return (false);
	for (size_t i = 1; i < nickname.length(); i++)
	{
		if (!isalnum(nickname[i]) && !isspecial(nickname[i]))
			return (false);
	}
	return (true);
}

bool Command::check_nickname_duplicate(std::string nickname, std::map<int, Client *> Clients)
{
	std::map<int, Client *>::iterator iter = Clients.begin();
	for (; iter != Clients.end(); iter++)
	{
		if (iter->second->get_nickname() == nickname)
			return (false);
	}
	return (true);
}

bool Command::check_realname(std::string realname)
{
	if (realname.length() == 0)
		return (false);
	if (realname[0] != ':')
		return (false);
	return (true);
}