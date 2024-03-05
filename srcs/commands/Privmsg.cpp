#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::privmsg(int fd, std::vector<std::string> command_vec)
{
	/* PRIVMSG(or /msg) <channel/nickname> <messages ...> */
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator client_iter = clients.find(fd);
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(client_iter->second);
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
		if ((*vec_iter)[0] == '#' || (*vec_iter)[0] == '&')	// send to channel
		{
			Channel *channel = _server.findChannel(*vec_iter);
			if (channel)	// if channel exists
			{
				if (command_vec.size() > 2 && checkBotCommand(command_vec[2]))
				{
					botCommand(fd, command_vec);
					return;
				}
				std::string message = channelMessage(2, command_vec);
				channelPRIVMSG(message, client_iter->second, channel);
			}
			else	// if channel not exists
			{
				err_nosuchchannel_403(client_iter->second, *vec_iter);
			}
		}
		else
		{
			std::map<int, Client>::iterator client = _server.findClient(*vec_iter);
			if (client != _server.getClients().end())
			{
				std::string message = channelMessage(2, command_vec);
				client->second.appendClientRecvBuf(":" + client_iter->second.getNickname() + " PRIVMSG " + client->second.getNickname() + " :" + message + "\r\n");
			}
			else
			{
				err_nosuchnick_401(client_iter->second, *vec_iter);
			}
		}
	}
}

bool Command::checkBotCommand(std::string command)
{
	if (!strcmp(command.c_str(), ":@BOT"))
		return true;
	return false;
}

void Command::botCommand(int fd, std::vector<std::string> command_vec)
{
	if (command_vec.size() < 3)
	{
		err_needmoreparams_461(_server.getClients().find(fd)->second);
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
		err_nosuchchannel_403(_server.getClients().find(fd)->second, command_vec[1]);
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
			err_needmoreparams_461(_server.getClients().find(fd)->second);
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
			err_needmoreparams_461(_server.getClients().find(fd)->second);
			return;
		}
		std::string commandName = command_vec[4];
		bot->delCommand(commandName);
	}
	else if (strcmp(command.c_str(), "do") == 0)
	{
		if (command_vec.size() < 5)
		{
			err_needmoreparams_461(_server.getClients().find(fd)->second);
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
