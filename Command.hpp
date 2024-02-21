#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "main.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"

class Client;
class Server;
class Channel;

class Command
{
private:
	/* OCCF */
	Command &operator=(const Command &);
	Command(const Command &);

	/* member variables */
	Server&	_server;

public:
	/* OCCF */
	Command(Server &server);
	~Command();

	/* member functions */
	void run(int);
	void pass(int, std::vector<std::string> command_vec);
	void nick(int, std::vector<std::string> command_vec);
	void user(int, std::vector<std::string> command_vec);
	void ping(int, std::vector<std::string> command_vec);
	void privmsg(int, std::vector<std::string> command_vec);
	void quit(int, std::vector<std::string> command_vec);
	void part(int, std::vector<std::string> command_vec);
	std::string channelMessage(int index, std::vector<std::string> command_vec);
	void channelPRIVMSG(std::string message, Client *client, Channel *channel);
	void channelPART(int, std::string channelName, std::vector<std::string> command_vec);
	void join(int fd, std::vector<std::string> command_vec);
	void msgToAllChannel(int target, std::string channelName, std::string command, std::string msg);
	std::string makeFullName(int fd);
	bool checkNicknameDuplicate(std::string, std::map<int, Client *>);
	bool checkNicknameValidate(std::string);
	bool checkRealname(std::string);
};

#endif