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
	std::vector<std::string> channel_list;
	int client_fd;
	bool is_regist_pass;
	bool is_regist_nick;
	bool is_regist_user;
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
	void clear_client_recv_buf();
	void clear_client_send_buf();
	std::string get_client_recv_buf();
	std::string get_client_send_buf();
	bool get_is_regist();
	bool get_pass_regist();
	bool get_nick_regist();
	bool get_user_regist();
	void set_pass_regist(bool);
	void set_nick_regist(bool);
	void set_user_regist(bool);
	int get_clientFd();
};

#endif