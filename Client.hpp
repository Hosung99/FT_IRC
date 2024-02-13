#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
private:
	std::string nickname;
	std::string username;
	std::string hostname;
	std::string servername;
	std::string realname;

	std::string client_recv_buf; // client가 받은 데이터
	std::string client_send_buf; // client가 보낼 데이터

	int client_fd;
	// todo channel list 추가 해야함
	// regist

public:
	Client(int clientfd);
	~Client();
	void set_nickname(std::string);
	void set_user(std::string username, std::string hostname, std::string servername, std::string realname);
	std::string get_nickname();
	std::string get_username();
	std::string get_hostname();
	std::string get_servername();
	std::string get_realname();
	void append_client_recv_buf(std::string);
	void append_client_send_buf(std::string);
	std::string get_client_recv_buf();
	std::string get_client_send_buf();
	int get_clientFd();
};

#endif