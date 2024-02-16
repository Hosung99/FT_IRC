#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "main.hpp"
#include "Client.hpp"

class Client;
class Server;

class Command
{
private:
	Command &operator=(const Command &);
	Command(const Command &);
	Server&	_server;

public:
	Command(Server &server);
	~Command();
	void run(int);
	void pass(int, std::vector<std::string> command_vec);
	void nick(int, std::vector<std::string> command_vec);
	void user(std::vector<std::string>, std::map<int, Client *>::iterator);
    bool check_nickname_duplicate(std::string, std::map<int, Client *>);
    bool check_nickname_validate(std::string);
    bool check_realname(std::string);
};

#endif