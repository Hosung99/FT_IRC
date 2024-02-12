#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
private:
	Client(const Client &);
	Client &operator=(const Client &);
	std::string nickname;
	std::string username;

public:
	Client();
	~Client();
};

#endif