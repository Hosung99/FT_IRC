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
	Server &_server;

public:
	/* OCCF */
	Command(Server &server);
	~Command();

	/* member functions */
	void run(int);
	void pass(int fd, std::vector<std::string> command_vec);
	void nick(int fd, std::vector<std::string> command_vec);
	void user(int fd, std::vector<std::string> command_vec);
	void ping(int fd, std::vector<std::string> command_vec);
	void privmsg(int fd, std::vector<std::string> command_vec);
	void quit(int fd, std::vector<std::string> command_vec);
	void part(int fd, std::vector<std::string> command_vec);
	void join(int fd, std::vector<std::string> command_vec);
	void kick(int fd, std::vector<std::string> command_vec);
	void mode(int fd, std::vector<std::string> command_vec);
	void topic(int fd, std::vector<std::string> command_vec);

	void botCommand(int fd, std::vector<std::string> command_vec);
	std::string channelMessage(int index, std::vector<std::string> command_vec);
	void channelPRIVMSG(std::string message, Client *client, Channel *channel);
	void channelPART(int, std::string channelName, std::vector<std::string> command_vec);
	void msgToAllChannel(int target, std::string channelName, std::string command, std::string msg);
	std::string makeFullName(int fd);
	void nameListMsg(int fd, std::string channelName);
	void topicMsg(int fd, std::string channelName);
	bool checkNicknameDuplicate(std::string, std::map<int, Client *>);
	bool checkNicknameValidate(std::string);
	bool checkRealname(std::string);
	bool checkBotCommand(std::string);
};

#endif