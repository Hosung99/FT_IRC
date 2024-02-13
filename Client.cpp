#include "Client.hpp"

Client::Client(int clientfd)
{
	this->client_fd = clientfd;
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