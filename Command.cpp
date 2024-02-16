#include "Command.hpp"

Command::Command() {}
Command::~Command() {}

static bool isspecial(char c)
{
	if (c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|')
		return (true);
	return (false);
}

void Command::run(int fd, std::map<int, Client *> Clients, std::string password, std::string command_string)
{
	std::istringstream iss(command_string);
	std::string buffer;
	std::map<int, Client *>::iterator iter;
	std::vector<std::string> command_vec;
	iter = Clients.find(fd);
	while (getline(iss, buffer, ' '))
	{
		command_vec.push_back(buffer);
	}
	if (!iter->second->get_is_regist())
	{
		if (command_vec[0] == "PASS")
		{
			pass(fd, password, command_vec, iter);
		}
		else if (command_vec[0] == "NICK")
		{
			nick(command_vec, Clients, iter);
		}
		else if (command_vec[0] == "USER")
		{
			user(command_vec, iter);
		}
		else
		{
			iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
			iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		}
	}
	else
	{
		// regist 된 client에 대한 명령어 처리
	}
}

void Command::pass(int fd, std::string password, std::vector<std::string> command_vec, std::map<int, Client *>::iterator iter)
{
	if (command_vec.size() < 2) // rfc문서상으론 multple pass commands가 가능하다는데, 그게 PASS command가 여러개가 들어오는건지, PASS command에 여러개의 password가 들어오는건지 모르겠음
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " ");
		iter->second->append_client_recv_buf("PASS: ");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_ALREADYREGIST);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (password != command_vec[1])
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_PASSWDMISMATCH);
		iter->second->append_client_recv_buf("\r\n");
		send(fd, iter->second->get_client_recv_buf().c_str(), iter->second->get_client_recv_buf().length(), 0);
		close(fd);
	}
	iter->second->set_pass_regist(true);
}

void Command::nick(std::vector<std::string> command_vec, std::map<int, Client *> Clients, std::map<int, Client *>::iterator iter)
{
	if (command_vec.size() < 2)
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NONICKNAMEGIVEN);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (!check_nickname_validate(command_vec[1]))
	{
		iter->second->append_client_recv_buf(command_vec[1] + " :");
		iter->second->append_client_recv_buf(ERR_ERRONEUSNICKNAME);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (!check_nickname_duplicate(command_vec[1], Clients))
	{
		iter->second->append_client_recv_buf(command_vec[1] + " :");
		iter->second->append_client_recv_buf(ERR_NICKNAMEINUSE);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (!iter->second->get_pass_regist())
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NOTREGISTERED);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
	if (!iter->second->get_nick_regist())
	{
		iter->second->set_nickname(command_vec[1]);
	}
	else
	{
		// 등록 되어있다면, 해당 클라이언트 fd의 닉네임을 바꾸겠단 뜻이다.
		// 해당 클라이언트가 속한 모든 채널을 돌며 닉네임을 바꾸었다고 알려준다.
	}
}

void Command::user(std::vector<std::string> command_vec, std::map<int, Client *>::iterator iter)
{
	if (command_vec.size() < 5)
	{
		iter->second->append_client_recv_buf(iter->second->get_nickname() + " :");
		iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		iter->second->append_client_recv_buf("\r\n");
		return;
	}
}

bool Command::check_nickname_validate(std::string nickname)
{
	if (nickname.length() == 0)
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