#include "Client.hpp"

Client::Client(int clientfd)
{
	this->client_fd = clientfd;
	nickname = "Client";
	is_regist_pass = false;
	is_regist_nick = false;
	is_regist_user = false;
}

Client::~Client()
{
	close(client_fd);
}

void Client::set_nickname(std::string nickname)
{
	this->nickname = nickname;
}

void Client::set_user(std::string username, std::string hostname, std::string servername, std::string realname)
{
	this->username = username;
	this->hostname = hostname;
	this->servername = servername;
	this->realname = realname;
}

std::string Client::get_nickname()
{
	return (this->nickname);
}

std::string Client::get_username()
{
	return (this->username);
}

std::string Client::get_hostname()
{
	return (this->hostname);
}

std::string Client::get_servername()
{
	return (this->servername);
}

std::string Client::get_realname()
{
	return (this->realname);
}

void Client::append_client_recv_buf(std::string client_recv_buf)
{
	this->client_recv_buf.append(client_recv_buf);
}

void Client::append_client_send_buf(std::string client_send_buf)
{
	this->client_send_buf.append(client_send_buf);
}

std::string Client::get_client_recv_buf()
{
	return (this->client_recv_buf);
}

std::string Client::get_client_send_buf()
{
	return (this->client_send_buf);
}

int Client::get_clientFd()
{
	return (this->client_fd);
}

bool Client::get_pass_regist()
{
	return (this->is_regist_pass);
}

bool Client::get_nick_regist()
{
	return (this->is_regist_nick);
}

bool Client::get_user_regist()
{
	return (this->is_regist_user);
}

void Client::set_pass_regist(bool is_regist_pass)
{
	this->is_regist_pass = is_regist_pass;
}

void Client::set_nick_regist(bool is_regist_nick)
{
	this->is_regist_nick = is_regist_nick;
}

void Client::set_user_regist(bool is_regist_user)
{
	this->is_regist_user = is_regist_user;
}

void Client::clear_client_recv_buf()
{
	this->client_recv_buf.clear();
}

void Client::clear_client_send_buf()
{
	this->client_send_buf.clear();
}

bool	Client::get_is_regist()
{
	return (is_regist_pass && is_regist_nick && is_regist_user);
}