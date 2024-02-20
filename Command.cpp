#include "Command.hpp"
#include "Channel.hpp"

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
			iter->second->append_client_recv_buf(":IRC 001 " + iter->second->get_nickname() + " :Welcome to the Interget Relay Network " + iter->second->get_nickname() + "!" + iter->second->get_username() + "@" + iter->second->get_hostname() + "\r\n");
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
		else if (command_vec[0] == "PRIVMSG")
			privmsg(fd, command_vec);
		else if (command_vec[0] == "QUIT")
			quit(fd, command_vec);
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
	for (size_t i = 4; i < command_vec.size(); i++)
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

void Command::privmsg(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
	{
		client_iter->second->append_client_recv_buf("461 :");
		client_iter->second->append_client_recv_buf(ERR_NEEDMOREPARAMS);
		return;
	}
	std::istringstream iss(command_vec[1]);
	std::string buffer;
	std::vector<std::string> vec;
	while (getline(iss, buffer, ','))
		vec.push_back(buffer);
	std::vector<std::string>::iterator vec_iter = vec.begin();
	for (; vec_iter != vec.end(); vec_iter++)
	{
		if ((*vec_iter)[0] == '#') // 채널에 메시지를 보낸다
		{
			Channel *channel = _server.findChannel(*vec_iter);
			if (channel) // 해당 채널을 찾음
			{
				std::string message = channelMessage(2, command_vec);
				channelPRIVMSG(message, client_iter->second, channel);
			}
			else
			{
				client_iter->second->append_client_recv_buf("403 " + *vec_iter + " :" + ERR_NOSUCHCHANNEL);
			}
		}
		else
		{
			// 유저에게 메시지를 보낸다
			Client *client = _server.findClient(*vec_iter);
			if (client) // 해당 유저를 찾음
			{
				std::string message = channelMessage(2, command_vec);
				client->append_client_recv_buf(":" + client_iter->second->get_nickname() + " PRIVMSG " + client->get_nickname() + " :" + message + "\r\n");
			}
			else
			{
				client_iter->second->append_client_recv_buf("401 " + *vec_iter + " :No such nickname\r\n");
			}
		}
	}
}

void Command::quit(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	std::vector<std::string> channelList = client_iter->second->getChannelList();
	std::vector<std::string>::iterator channel_iter = channelList.begin();
	for (; channel_iter != channelList.end(); channel_iter++)
	{
		Channel *channel = _server.findChannel(*channel_iter);
		channel->removeClientFdList(fd);
		if (channel)
		{
			channelPART(fd, channel->getChannelName(), command_vec);
		}
		if (channel->getClientFdList().empty())
		{
			_server.removeChannel(channel->getChannelName());
			delete channel;
		}
		else
		{
			channel->setOperator(channel->getClientFdList().front());
		}
	}
	clients.erase(fd);
	close(fd);
	delete client_iter->second;
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

std::string Command::channelMessage(int index, std::vector<std::string> command_vec)
{
	std::string message;
	for (size_t i = index; i < command_vec.size(); i++)
	{
		message += command_vec[i];
		if (i != command_vec.size() - 1)
			message += " ";
	}
	return (message);
}

void Command::channelPRIVMSG(std::string message, Client *client, Channel *channel)
{
	std::vector<int> fdList = channel->getClientFdList();
	std::vector<int>::iterator fd_iter = fdList.begin();
	for (; fd_iter != fdList.end(); fd_iter++)
	{
		if (*fd_iter != client->get_clientFd())
		{
			Client *target = _server.getClients().find(*fd_iter)->second;
			target->append_client_recv_buf(":" + client->get_nickname() + " PRIVMSG " + channel->getChannelName() + " :" + message + "\r\n");
		}
	}
}

void Command::channelPART(int fd, std::string channelName, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	Channel *channel = _server.findChannel(channelName);
	std::vector<int> fdList = channel->getClientFdList();
	std::vector<int>::iterator fd_iter = fdList.begin();
	std::string message = channelMessage(1, command_vec);
	for (; fd_iter != fdList.end(); fd_iter++)
	{
		if (*fd_iter != fd)
		{
			Client *target = _server.getClients().find(*fd_iter)->second;
			target->append_client_recv_buf(":" + target->get_nickname() + +"!" + target->get_username() + "@" + target->get_servername() + client_iter->second->get_nickname() + " PART " + channel->getChannelName() + " " + message + "\r\n");
		}
	}
}