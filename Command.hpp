#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "main.hpp"
#include "Client.hpp"

class Client;

class Command
{
private:
	Command &operator=(const Command &);
	Command(const Command &);
	// Server *server;

public:
	// Command(Server &server);
	Command();
	~Command();
	void run(int, std::map<int, Client *>, std::string, std::string);
	void pass(int, std::string, std::vector<std::string>, std::map<int, Client *>, std::map<int, Client *>::iterator);
	void nick(int, std::vector<std::string>, std::map<int, Client *>, std::map<int, Client *>::iterator);
	void user(int, std::vector<std::string>, std::map<int, Client *>, std::map<int, Client *>::iterator);
	bool check_nickname_duplicate(std::string, std::map<int, Client *>);
	bool check_nickname_validate(std::string);
	bool check_realname(std::string);
};

#endif