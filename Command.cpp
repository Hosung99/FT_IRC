#include "Command.hpp"

Command::Command(Server &server) : _server(server) {}
Command::~Command() {}

static bool isspecial(char c)
{
	if (c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|' || c == '_')
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
		else if (command_vec[0] == "MODE")
			mode(fd, command_vec);
		else if (command_vec[0] == "TOPIC")
			topic(fd, command_vec);
		else if (command_vec[0] == "INVITE")
			invite(fd, command_vec);
	}
}

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
	if (command_vec[1] == "_")
	{
		while(1)
		{
			Client *client = _server.findClient(command_vec[1]);
			if (client != NULL)
				command_vec[1] = client->getNickname() + "_";
			else
				break ;
		}
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
	if (old_nickname == "Client")
		old_nickname = iter->second->getNickname();
	std::vector<std::string> channelList = iter->second->getChannelList();
	std::vector<std::string>::iterator channel_iter = channelList.begin();
	for (; channel_iter != channelList.end(); channel_iter++)
	{
		Channel *channel = _server.findChannel(*channel_iter);
		msgToAllChannel(fd, channel->getChannelName(), "NICK", old_nickname + " " + command_vec[1]);
	}
	iter->second->setNickname(command_vec[1]);
	iter->second->appendClientRecvBuf(":" + old_nickname + " NICK " + iter->second->getNickname() + "\r\n");
	iter->second->setNickRegist(true);
}

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
			Client *client = _server.findClient(*vec_iter);
			if (client)
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
		channel->removeOperatorFd(fd);
		if (channel)
		{
			msgToAllChannel(fd, channel->getChannelName(), "QUIT", channelMessage(1, command_vec));
		}
		if (channel->getClientFdList().size() == 1)
		{
			_server.removeChannel(channel->getChannelName());
			delete channel;
		}
		else
		{
			std::vector<int> fdList = channel->getClientFdList();
			std::vector<int>::iterator fd_iter = fdList.begin();
			fd_iter++;
			channel->addOperatorFd(*fd_iter);
		}
	}
	client_iter->second->setRegist(false);
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
			channel->removeOperatorFd(fd);
			client_iter->second->removeChannel(*channelList);
			if (channel->getClientFdList().size() == 1)
			{
				_server.removeChannel(channel->getChannelName());
				delete channel;
			}
			else
			{
				std::vector<int> fdList = channel->getClientFdList();
				std::vector<int>::iterator fd_iter = fdList.begin();
				fd_iter++;
				channel->addOperatorFd(*fd_iter);
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
		if (!isalnum(nickname[i]) && !isspecial(nickname[i]))
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
		{
			return (false);
		}
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
	if (command_vec.size() < 2)
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
		_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	std::vector<std::string> joinChannel = split(command_vec[1], ',');
	std::vector<std::string>::iterator iter = joinChannel.begin();
	std::vector<std::string> joinKey;
	std::vector<std::string>::iterator keyIter;
	if (command_vec.size() > 2)
	{
		joinKey = split(command_vec[2], ',');
		keyIter = joinKey.begin();
	}
	std::map<int, Client *> clients = _server.getClients();
	Client *client = clients.find(fd)->second;
	while (iter != joinChannel.end())
	{
		if ((*iter)[0] != '#' && (*iter)[0] != '&')
		{
			client->appendClientRecvBuf("403 " + *iter + " :" + ERR_NOSUCHCHANNEL);
			iter++;
			if (command_vec.size() > 2 || keyIter != joinKey.end())
				keyIter++;
			continue;
		}
		std::map<std::string, Channel *> channelList = _server.getChannelList();
		std::map<std::string, Channel *>::iterator channelIt = channelList.find(*iter);
		if (channelIt != channelList.end()) // 채널이 있다면
		{
			// 해당 클라이언트를 채널에 넣어준다.
			Channel *channel = channelIt->second;
			if (channel->checkMode(INVITE)) // INVITE 모드가 켜져있다면
			{
				if (!channel->checkInvite(fd))
				{
					client->appendClientRecvBuf("473 " + client->getNickname() + " " + *iter + " :" + ERR_INVITEONLYCHAN);
					iter++;
					if (command_vec.size() > 2 || keyIter != joinKey.end())
						keyIter++;
					continue;
				}
			}
			if (channel->checkMode(KEY)) // KEY 모드가 켜져있다면
			{
				if (command_vec.size() <= 2 || keyIter == joinKey.end() || !channel->checkKey(*keyIter)) // 인자에 키가 없거나 키가 틀리다면
				{
					client->appendClientRecvBuf("475 " + client->getNickname() + " " + *iter + " :" + ERR_BADCHANNELKEY);
					iter++;
					if (command_vec.size() > 2 || keyIter != joinKey.end())
						keyIter++;
					continue;
				}
			}
			std::string channelName = (*channelIt).second->getChannelName();
			(*channelIt).second->appendClientFdList(fd);
			client->appendChannelList(channelName);		  // 클라이언트에 채널을 추가 해준다.
			msgToAllChannel(fd, channelName, "JOIN", ""); // 채널에 JOIN 메시지를 보내준다.
			topicMsg(fd, channelName);
		}
		else // 채널이 없다면
		{
			_server.appendNewChannel(*iter, fd);				// 서버에 채널을 추가 해준다.
			_server.findChannel(*iter)->appendClientFdList(fd); // 해당 클라이언트를 채널에 넣어준다.
			_server.findChannel(*iter)->appendClientFdList(-1);
			client->appendChannelList(*iter);
			msgToAllChannel(fd, *iter, "JOIN", "");
			_server.findChannel(*iter)->addOperatorFd(fd);
		}
		nameListMsg(fd, *iter);
		msgToAllChannel(-1, *iter, "PRIVMSG", _server.findChannel(*iter)->getBot()->introduce());
		iter++;
		if (command_vec.size() > 2 || keyIter != joinKey.end())
			keyIter++;
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
	if (!_server.findChannel(*vec_iter)->checkOperator(fd))
	{
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
				return;
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

void Command::mode(int fd, std::vector<std::string> command_vec)
{
	// parsing
	Client *client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 2)
	{
		client->appendClientRecvBuf("461 :");
		client->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (client->getNickname() != command_vec[1] && channel == NULL) // 채널이 없을때
	{
		client->appendClientRecvBuf("403 " + client->getNickname() + " " + command_vec[1] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (channel != NULL && command_vec.size() == 2) // 채널의 모드를 보여줄때
	{
		client->appendClientRecvBuf("324 " + client->getNickname() + " " + command_vec[1] + " +" + channel->getMode() + "\r\n");
		return;
	}
	if (channel != NULL && !channel->checkOperator(fd)) // 채널의 오퍼레이터가 아닐때
	{
		client->appendClientRecvBuf("482 " + client->getNickname() + command_vec[1] + " :" + ERR_CHANOPRIVSNEEDED);
		return;
	}
	std::string mode = command_vec[2];
	char sign = mode[0];
	if (mode.length() == 1 || (sign != '+' && sign != '-'))
	{
		// ERR_UNKNOWNMODE
		return;
	}
	std::string msg = "";
	msg += mode[0];
	for (size_t i = 1; i < mode.length(); i++)
	{
		if (mode[i] == 'i')
		{
			if (client->getNickname() == command_vec[1])
			{
				client->appendClientRecvBuf("324 " + client->getNickname() + " " + command_vec[1] + " +i" + "\r\n");
				return;
			}
			channel->setMode(INVITE, sign);
		}
		else if (mode[i] == 't')
		{
			channel->setMode(TOPIC, sign);
		}
		else if (mode[i] == 'k')
		{
			channel->setMode(KEY, sign);
			if (sign == '+')
			{
				channel->setKey(command_vec[3]);
			}
		}
		else if (mode[i] == 'l')
		{
			channel->setMode(LIMIT, sign);
			if (sign == '+')
			{
				channel->setLimit(atoi(command_vec[3].c_str()));
			}
		}
		else if (mode[i] == 'o')
		{
			Client *target = _server.findClient(command_vec[3]);
			if (target == NULL) // 해당 클라이언트가 서버에 없을때
			{
				client->appendClientRecvBuf("401 " + client->getNickname() + " " + command_vec[3] + " :" + ERR_NOSUCHNICK);
				return;
			}
			else // 클라이언트가 서버에는 있음
			{
				if (!channel->checkClientInChannel(target->getClientFd())) // 채널에는 없음
				{
					client->appendClientRecvBuf("441 " + command_vec[3] + " " + command_vec[1] + " :" + ERR_USERNOTINCHANNEL);
					return;
				}
				else if (sign == '+') // 오퍼레이터 추가
				{
					channel->addOperatorFd(target->getClientFd());
				}
				else if (sign == '-') // 오퍼레이터 삭제
				{
					channel->removeOperatorFd(target->getClientFd());
				}
			}
		}
		else
		{
			// ERR_UNKNOWNMODE
			client->appendClientRecvBuf("472 " + client->getNickname() + " " + mode[i] + " :" + ERR_UNKNOWNMODE);
			continue;
		}
		msg += mode[i];
		if (command_vec.size() > 3)
		{
			msg += " " + command_vec[3];
		}
		msgToAllChannel(fd, command_vec[1], "MODE", msg);
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

void Command::topicMsg(int fd, std::string channelName)
{
	std::map<std::string, Channel *> channelList = _server.getChannelList();
	Channel *channel = channelList.find(channelName)->second;
	std::string topic = channel->getTopic();
	if (topic.length() == 0)
	{
		return;
	}
	topic = topic.substr(1, topic.length() - 1);
	Client *client = _server.getClients().find(fd)->second;
	client->appendClientRecvBuf("332 " + client->getNickname() + " " + channelName + " :" + topic + "\r\n");
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
		if (channel->checkOperator(client->getClientFd()))
			message += "@";
		message += client->getNickname();
		if (iter != clientFdList.end() - 1)
			message += " ";
		iter++;
	}
	Client *client = _server.getClients().find(fd)->second;
	client->appendClientRecvBuf("353 " + client->getNickname() + " = " + channelName + " :" + message + "\r\n");
	client->appendClientRecvBuf("366 " + client->getNickname() + " " + channelName + " :End of NAMES list.\r\n");
}

void Command::botCommand(int fd, std::vector<std::string> command_vec)
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
	if (!strcmp(command.c_str(), ":@BOT"))
		return true;
	return false;
}

void Command::topic(int fd, std::vector<std::string> command_vec)
{
	if (command_vec.size() < 2)
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("461 :");
		_server.getClients().find(fd)->second->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (channel == NULL)
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("403 " + command_vec[1] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	for (; iter != clientFdList.end(); iter++)
	{
		if (*iter == fd)
		{
			break;
		}
	}
	if (iter == clientFdList.end())
	{
		_server.getClients().find(fd)->second->appendClientRecvBuf("442 " + command_vec[1] + " :" + ERR_NOTONCHANNEL);
		return;
	}
	if (channel->checkMode(TOPIC))
	{
		if (channel->checkOperator(fd) == false)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("482 " + _server.getClients().find(fd)->second->getNickname() + " " + command_vec[1] + " :" + ERR_CHANOPRIVSNEEDED);
			return;
		}
	}
	if (command_vec.size() == 2)
	{
		if (channel == NULL)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("403 " + command_vec[1] + " :" + ERR_NOSUCHCHANNEL);
			return;
		}
		if (channel->getTopic().length() == 0)
		{
			_server.getClients().find(fd)->second->appendClientRecvBuf("331 " + _server.getClients().find(fd)->second->getNickname() + " " + command_vec[1] + " :" + RPL_NOTOPIC);
			return;
		}
		_server.getClients().find(fd)->second->appendClientRecvBuf("332 " + _server.getClients().find(fd)->second->getNickname() + " " + command_vec[1] + " :" + channel->getTopic() + "\r\n");
	}
	else
	{
		if (command_vec[2] == ":")
		{
			channel->setTopic("");
		}
		else
		{
			std::string topic = command_vec[2];
			for (size_t i = 3; i < command_vec.size(); i++)
			{
				topic += " " + command_vec[i];
			}
			channel->setTopic(topic);
		}
		msgToAllChannel(fd, command_vec[1], "TOPIC", channel->getTopic());
	}
}

void Command::invite(int fd, std::vector<std::string> command_vec)
{
	// INVITE <nickname> <channel>
	Client *client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 3)
	{
		client->appendClientRecvBuf("461 :");
		client->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Client *target = _server.findClient(command_vec[1]);
	if (target == NULL)
	{
		client->appendClientRecvBuf("401 " + client->getNickname() + " " +  command_vec[1] + " :" + ERR_NOSUCHNICK);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[2]);
	if (channel == NULL)
	{
		client->appendClientRecvBuf("403 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (!channel->checkClientInChannel(client->getClientFd()))
	{
		client->appendClientRecvBuf("442 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_NOTONCHANNEL);
		return;
	}
	if (!channel->checkOperator(client->getClientFd()))
	{
		client->appendClientRecvBuf("482 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_CHANOPRIVSNEEDED);
		return;
	}
	if (channel->checkClientInChannel(target->getClientFd()))
	{
		client->appendClientRecvBuf("443 " + client->getNickname() + " " + command_vec[1] + " " + command_vec[2] + " :" + ERR_USERONCHANNEL);
		return;
	}
	target->appendClientRecvBuf(":" + client->getNickname() + " INVITE " + target->getNickname() + " " + command_vec[2] + "\r\n");
	client->appendClientRecvBuf("341 " + client->getNickname() + " " + command_vec[1] + " " + command_vec[2] + " :" + RPL_INVITING);
	channel->appendInviteFdList(target->getClientFd());
}
