#ifndef SERVER_HPP
#define SERVER_HPP

#include "../main.hpp"
#include "Command.hpp"
#include "Channel.hpp"

class Client;
class Channel;
class Command;

class Server
{
public:
	/* OCCF */
	Server(char *, char *);
	~Server();

	Client *_bot;

	/* member functions */
	void run(void);
	std::map<int, Client *>& getClients(void);
	std::string getPassword(void);
	std::string getMessage(int);
	std::map<std::string, Channel *>& getChannelList(void);
	Channel *findChannel(std::string channelName);
	Client *findClient(std::string clientName);
	void removeChannel(std::string channelName);
	void appendNewChannel(std::string& channelName, int fd);

private:
	/* OCCF */
	Server();
	Server(const Server &);
	Server &operator=(const Server &);

	/* member variables */
	Command *_command;
	std::map<int, Client *> _clients;
	std::map<std::string, Channel *> _channelList;
	std::string _password;
	unsigned short int _portNum;
	int _serverSock;
	int _clientSock;
	struct sockaddr_in _serverAddr;
	struct sockaddr_in _clientAddr;
	socklen_t _clientAddrSize;
	std::string _message[BUF_SIZE];
	int _strLen;
	int _fdCnt;
	struct pollfd _fds[256];

	/* member functions */
	void execute();
	void setServerSock();
	void setServerAddr();
	void setServerBind();
	void setServerListen();
	unsigned short int setPortNum(char *);
	std::string setPassword(char *);
	int getServerSock();
	int recvMessage(int);
	void addClient(int);
	bool checkMessageEnds(int);
	void doCommand(int);
	void setBot();
};

#endif