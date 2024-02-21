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
	if (!iter->second->getIsRegist())
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
			iter->second->appendClientRecvBuf(iter->second->getNickname() + " :");
			iter->second->appendClientRecvBuf(ERR_NOTREGISTERED);
			clients.erase(fd);
			close(fd);
			delete iter->second;
		}
		if (iter->second && iter->second->getIsRegist())
		{
			iter->second->appendClientRecvBuf(":IRC 001 " + iter->second->getNickname() + " :Welcome to the Interget Relay Network " + iter->second->getNickname() + "!" + iter->second->getUsername() + "@" + iter->second->getHostname() + "\r\n");
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
		else if (command_vec[0] == "PART")
			part(fd, command_vec);
		else if (command_vec[0] == "JOIN")
			join(fd, command_vec);
		else if (command_vec[0] == "KICK")
			kick(fd, command_vec);
		// else
		// {
		// 	iter->second->appendClientRecvBuf(iter->second->getNickname() + " :");
		// 	iter->second->appendClientRecvBuf(ERR_NOTREGISTERED);
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
	if (iter->second->getPassRegist())
	{
		iter->second->appendClientRecvBuf("462 :");
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
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	iter->second->setPassRegist(true);
}

void Command::nick(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (!iter->second->getPassRegist())
	{
		iter->second->appendClientRecvBuf("451 :");
		iter->second->appendClientRecvBuf(ERR_NOTREGISTERED);
		send(fd, iter->second->getClientRecvBuf().c_str(), iter->second->getClientRecvBuf().length(), 0);
		clients.erase(fd);
		close(fd);
		delete iter->second;
		return;
	}
	if (command_vec.size() < 2)
	{
		iter->second->appendClientRecvBuf("431 :");
		iter->second->appendClientRecvBuf(ERR_NONICKNAMEGIVEN);
		return;
	}
	if (!checkNicknameValidate(command_vec[1]))
	{
		iter->second->appendClientRecvBuf("432 :");
		iter->second->appendClientRecvBuf(ERR_ERRONEUSNICKNAME);
		iter->second->appendClientRecvBuf("/NICK <nickname> First Letter is not digit and length is under 10.\r\n");
		return;
	}
	if (!checkNicknameDuplicate(command_vec[1], _server.getClients()))
	{
		iter->second->appendClientRecvBuf("433 :");
		iter->second->appendClientRecvBuf(ERR_NICKNAMEINUSE);
		return;
	}
	std::string old_nickname = iter->second->getNickname();
	iter->second->setNickname(command_vec[1]);
	if (old_nickname == "Client")
		old_nickname = iter->second->getNickname();
	iter->second->appendClientRecvBuf(":" + old_nickname + " NICK " + iter->second->getNickname() + "\r\n");
	iter->second->setNickRegist(true);
	// + 해당 클라이언트가 속한 모든 채널을 돌며 닉네임을 바꾸었다고 알려준다.
}

void Command::user(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator iter = clients.find(fd);
	if (iter->second->getUserRegist())
	{
		iter->second->appendClientRecvBuf("462 :");
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
		delete iter->second;
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

void Command::privmsg(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
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
	for (; vec_iter != vec.end(); vec_iter++)
	{
		if ((*vec_iter)[0] == '#' || (*vec_iter)[0] == '&') // 채널에 메시지를 보낸다
		{
			Channel *channel = _server.findChannel(*vec_iter);
			if (channel) // 해당 채널을 찾음
			{
				if (command_vec.size() > 2 && checkBotCommand(command_vec[2]))
				{
					botCommand(fd, command_vec);
					return;
				}
				std::string message = channelMessage(2, command_vec);
				channelPRIVMSG(message, client_iter->second, channel);
			}
			else
			{
				client_iter->second->appendClientRecvBuf("403 " + *vec_iter + " :" + ERR_NOSUCHCHANNEL);
			}
		}
		else
		{
			// 유저에게 메시지를 보낸다
			Client *client = _server.findClient(*vec_iter);
			if (client) // 해당 유저를 찾음
			{
				std::string message = channelMessage(2, command_vec);
				client->appendClientRecvBuf(":" + client_iter->second->getNickname() + " PRIVMSG " + client->getNickname() + " :" + message + "\r\n");
			}
			else
			{
				client_iter->second->appendClientRecvBuf("401 " + *vec_iter + " :No such nickname\r\n");
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
			delete channel->getBot();
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

void Command::part(int fd, std::vector<std::string> command_vec)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
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
	for (; vec_iter != vec.end(); vec_iter++)
	{
		std::vector<std::string>::iterator channelList = client_iter->second->findChannel(*vec_iter);
		if (channelList != client_iter->second->getChannelList().end())
		{
			Channel *channel = _server.findChannel(*channelList);
			msgToAllChannel(fd, *vec_iter, "PART", channelMessage(2, command_vec));
			channel->removeClientFdList(fd);
			client_iter->second->removeChannel(*channelList);
			if (channel->getClientFdList().empty())
			{
				_server.removeChannel(channel->getChannelName());
				delete channel->getBot();
				delete channel;
			}
			else
			{
				channel->setOperator(channel->getClientFdList().front());
			}
		}
		else
		{
			if (_server.findChannel(*vec_iter))
			{
				client_iter->second->appendClientRecvBuf("442 " + *vec_iter + " :" + ERR_NOTONCHANNEL);
			}
			else
			{
				client_iter->second->appendClientRecvBuf("403 " + *vec_iter + " :" + ERR_NOSUCHCHANNEL);
			}
		}
	}
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
		if (iter->second->getNickname() == nickname)
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
	std::string message = "";
	for (size_t i = index; i < command_vec.size(); i++)
	{
		message += command_vec[i];
		if (i != command_vec.size() - 1)
			message += " ";
	}
	if (message.length() == 0)
		message = "NO REASON";
	return (message);
}

void Command::channelPRIVMSG(std::string message, Client *client, Channel *channel)
{
	std::vector<int> fdList = channel->getClientFdList();
	std::vector<int>::iterator fd_iter = fdList.begin();
	for (; fd_iter != fdList.end(); fd_iter++)
	{
		if (*fd_iter != client->getClientFd())
		{
			Client *target = _server.getClients().find(*fd_iter)->second;
			target->appendClientRecvBuf(":" + client->getNickname() + " PRIVMSG " + channel->getChannelName() + " :" + message + "\r\n");
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
			target->appendClientRecvBuf(":" + target->getNickname() + "!" + target->getUsername() + "@" + target->getServername() + client_iter->second->getNickname() + " PART " + channel->getChannelName() + " " + message + "\r\n");
		}
	}
}

void Command::join(int fd, std::vector<std::string> command_vec)
{
	std::vector<std::string> joinChannel = split(command_vec[1], ',');
	std::vector<std::string>::iterator iter = joinChannel.begin();
	std::map<int, Client *> clients = _server.getClients();
	Client *client = clients.find(fd)->second;
	while (iter != joinChannel.end())
	{
		if ((*iter)[0] != '#')
		{
			// ERR_NOSUCHCHANNEL
			return;
		}
		std::map<std::string, Channel *> channelList = _server.getChannelList();
		std::map<std::string, Channel *>::iterator channelIt = channelList.find(*iter);
		if (channelIt != channelList.end()) // 채널이 있다면
		{
			// 해당 클라이언트를 채널에 넣어준다.
			std::string channelName = (*channelIt).second->getChannelName();
			(*channelIt).second->appendClientFdList(fd);
			client->appendChannelList(channelName);		  // 클라이언트에 채널을 추가 해준다.
			msgToAllChannel(fd, channelName, "JOIN", ""); // 채널에 JOIN 메시지를 보내준다.
		}
		else // 채널이 없다면
		{
			_server.appendNewChannel(*iter, fd);				// 서버에 채널을 추가 해준다.
			_server.findChannel(*iter)->appendClientFdList(fd); // 해당 클라이언트를 채널에 넣어준다.
			_server.findChannel(*iter)->appendClientFdList(-1);
			client->appendChannelList(*iter);
			msgToAllChannel(fd, *iter, "JOIN", "");
			_server.findChannel(*iter)->setOperator(fd);
		}
		nameListMsg(fd, *iter);
		msgToAllChannel(fd, *iter, "PRIVMSG", _server.findChannel(*iter)->getBot()->introduce());
		iter++;
	}
}

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
	if (_server.findChannel(*vec_iter)->getOperatorFd() != fd)
	{
		std::cout << "operator fd : " << _server.findChannel(*vec_iter)->getOperatorFd() << std::endl;
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
			if (target->getNickname() == client_iter->second->getNickname())
			{
				return ;
			}
			if (target == NULL)
			{
				client_iter->second->appendClientRecvBuf("401 " + command_vec[2] + " :" + ERR_NOSUCHNICK);
			}
			else
			{
				if (!channel->checkClientInChannel(target->getClientFd()))
				{
					client_iter->second->appendClientRecvBuf("441 " + command_vec[2] + " " + *vec_iter + " :" + ERR_USERNOTINCHANNEL);
				}
				else
				{
					msgToAllChannel(fd, *vec_iter, "KICK", command_vec[2]);
					channel->removeClientFdList(target->getClientFd());
					target->removeChannel(*vec_iter);
				}
			}
		}
	}
}

void Command::msgToAllChannel(int target, std::string channelName, std::string command, std::string msg)
{
	std::map<std::string, Channel *> channelList = _server.getChannelList();
	if (channelList.find(channelName) == channelList.end())
	{
		// ERR_NOSUCHCHANNEL
		return;
	}
	Channel *channel = channelList.find(channelName)->second;
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	while (iter != clientFdList.end())
	{
		Client *client = _server.getClients().find(*iter)->second;
		client->appendClientRecvBuf(makeFullName(target) + " " + command + " " + channelName + " " + msg + "\r\n");
		iter++;
	}
}

std::string Command::makeFullName(int fd)
{
	std::map<int, Client *> clients = _server.getClients();
	std::map<int, Client *>::iterator clientIt = clients.find(fd);
	Client *client = clientIt->second;
	std::string temp = (":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getServername());
	return temp;
}

void Command::nameListMsg(int fd, std::string channelName)
{
	std::map<std::string, Channel *> channelList = _server.getChannelList();
	if (channelList.find(channelName) == channelList.end())
	{
		// ERR_NOSUCHCHANNEL
		return;
	}
	Channel *channel = channelList.find(channelName)->second;
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	std::string message;
	while (iter != clientFdList.end())
	{
		Client *client = _server.getClients().find(*iter)->second;
		message += client->getNickname();
		if (iter != clientFdList.end() - 1)
			message += " ";
		iter++;
	}
	Client *client = _server.getClients().find(fd)->second;
	client->appendClientRecvBuf("353 " + client->getNickname() + " = " + channelName + " :" + message + "\r\n");
	client->appendClientRecvBuf("366 " + client->getNickname() + " " + channelName + " :End of NAMES list.\r\n");
}

void Command::botCommand(int fd, std::vector<std::string>command_vec)
{
	if (command_vec.size() < 3)
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
		_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (command_vec.size() == 3)
	{
		msgToAllChannel(-1, channel->getChannelName(), "PRIVMSG", _server.findChannel(channel->getChannelName())->getBot()->introduce());
		return;
	}
	if (channel == NULL)
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("403 " + command_vec[1] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	std::string command = command_vec[3];
	Bot *bot = channel->getBot();
	if (!strcmp(command.c_str(), "list"))
	{
		std::vector<std::string> bot_command = bot->listCommand();
		std::vector<std::string>::iterator iter = bot_command.begin();
		std::string message = "BOT COMMAND LIST : ";
		while (iter != bot_command.end())
		{
			message += *iter;
			if (iter != bot_command.end() - 1)
				message += ", ";
			iter++;
		}
		msgToAllChannel(-1, channel->getChannelName(), "PRIVMSG", message);
	}
	else if (strcmp(command.c_str(), "add") == 0)
	{
		if (command_vec.size() < 6)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
			_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
			return;
		}
		std::string commandName = command_vec[4];
		std::string commandContent = command_vec[5];
		bot->addCommand(commandName, commandContent);
	}
	else if (strcmp(command.c_str(), "del") == 0)
	{
		if (command_vec.size() < 5)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
			_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
			return;
		}
		std::string commandName = command_vec[4];
		bot->delCommand(commandName);
	}
	else if (strcmp(command.c_str(), "do") == 0)
	{
		if (command_vec.size() < 5)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
			_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
			return;
		}
		std::string commandName = command_vec[4];
		std::string response = bot->doCommand(commandName);
		msgToAllChannel(-1, channel->getChannelName(), "PRIVMSG", response);
	}
	else
	{
		msgToAllChannel(-1, channel->getChannelName(), "PRIVMSG", "BOT COMMAND NOT FOUND");
	}
}

bool Command::checkBotCommand(std::string command)
{
	if (command.length() < 4)
		return false;
	if (command.compare("@BOT"))
		return true;
	return false;
}