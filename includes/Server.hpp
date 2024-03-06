#ifndef SERVER_HPP
# define SERVER_HPP

# include "../main.hpp"
# include "./Command.hpp"
# include "./Channel.hpp"

class Client;
class Channel;
class Command;

class Server
{
public:
	/* OCCF */
	Server(char *, char *);
	~Server();

	/* member variables */
	Client								*_bot;

	/* member functions */
	// getter
	std::map<int, Client>&				getClients();
	std::string							getPassword();
	std::string							getMessage(int);
	std::map<std::string, Channel *>&	getChannelList();
	// others
	void								run();
	Channel								*findChannel(std::string);
	std::map<int, Client>::iterator		findClient(std::string);
	void								removeChannel(std::string);
	void								appendNewChannel(std::string&, int);

private:
	/* OCCF */
	Server();
	Server(const Server &);
	Server &operator=(const Server &);

	/* member variables */
	// server socket
	std::string							_password;
	unsigned short int					_portNum;
	int									_serverSock;
	struct sockaddr_in					_serverAddr;
	int									_fdCnt;
	struct pollfd						_fds[256];
	// others
	int									_clientSock;
	struct sockaddr_in					_clientAddr;
	socklen_t							_clientAddrSize;
	std::map<int, Client>				_clients;
	std::map<std::string, Channel *>	_channelList;
	std::string							_message[BUF_SIZE];
	int									_strLen;
	Command								*_command;

	/* member functions */
	// init
	unsigned short int					setPortNum(char *);
	std::string							setPassword(char *);
	void								setServerSock();
	void								setServerAddr();
	void								setServerBind();
	void								setServerListen();
	void								setBot();
	// others
	void								execute();
	int									recvMessage(int);
	void								addClient(int);
	bool								checkMessageEnds(int);
	void								doCommand(int);
};

#endif