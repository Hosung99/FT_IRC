#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"

class Client;

class Server
{
public:
	Server(char *, char *);
	~Server();
	void run();

private:
	Server();
	Server(const Server &);
	Server &operator=(const Server &);
	std::map<int, Client *> Clients;
	void execute();
	void set_server_sock();
	void set_server_addr();
	void set_server_bind();
	void set_server_listen();
	unsigned short int set_port_num(char *);
	std::string set_password(char *);
	int get_server_sock();
	int get_data(int);
	void add_client(int);
	bool check_message_ends(int);
	void do_command(int);
	std::string password;
	unsigned short int port_num;
	int server_sock;
	int client_sock;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size;
	std::string message[BUF_SIZE];
	int str_len;
	int fd_cnt;
	struct pollfd fds[256];
};

#endif