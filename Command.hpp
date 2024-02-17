#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "main.hpp"
#include "Client.hpp"

class Client;
class Server;

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
	bool checkNicknameDuplicate(std::string, std::map<int, Client *>);
	bool checkNicknameValidate(std::string);
	bool checkRealname(std::string);
};

#endif