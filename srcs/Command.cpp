#include "../includes/Command.hpp"
#include "../includes/Util.hpp"

/* constructor */
Command::Command(Server &server) : _server(server) {}

/* destructor */
Command::~Command() {}


/* verify commands */
void Command::run(int fd)
{
	std::istringstream iss(_server.getMessage(fd));
	std::string buffer;
	std::map<int, Client>::iterator iter;
	std::vector<std::string> command_vec;
	std::map<int, Client>& clients = _server.getClients();
	iter = clients.find(fd);
	while (getline(iss, buffer, ' '))
	{
		std::size_t endPos = buffer.find_last_not_of("\r\n");
		command_vec.push_back(buffer.substr(0, endPos + 1));
	}
	if (iter != clients.end() && !iter->second.getIsRegist())	// clients authentication
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
			iter->second.appendClientRecvBuf(iter->second.getNickname() + " :");
			iter->second.appendClientRecvBuf(ERR_NOTREGISTERED);
			send(fd, iter->second.getClientRecvBuf().c_str(), iter->second.getClientRecvBuf().length(), 0);
			iter->second.clearClient();
			clients.erase(fd);
			close(fd);
		}
		iter = clients.find(fd);
		if (iter != clients.end())
		{
			if (iter->second.getIsRegist())
			{
				iter->second.appendClientRecvBuf(":IRC 001 " + iter->second.getNickname() + " :Welcome to the Interget Relay Network " + iter->second.getNickname() + "!" + iter->second.getUsername() + "@" + iter->second.getHostname() + "\r\n");
			}
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
