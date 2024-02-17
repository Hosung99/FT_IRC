#include "Client.hpp"

Client::Client(int clientfd)
{
	this->_clientFd = clientfd;
	_nickName = "Client";
	_isRegistPass = false;
	_isRegistNick = false;
	_isRegistUser = false;
}

Client::~Client()
{
	close(_clientFd);
}

void Client::set_nickname(std::string nickname)
{
	this->_nickName = nickname;
}

void Client::set_user(std::string username, std::string hostname, std::string servername, std::string realname)
{
	this->_userName = username;
	this->_hostName = hostname;
	this->_serverName = servername;
	this->_realName = realname;
}

std::string Client::get_nickname()
{
	return (this->_nickName);
}

std::string Client::get_username()
{
	return (this->_userName);
}

std::string Client::get_hostname()
{
	return (this->_hostName);
}

std::string Client::get_servername()
{
	return (this->_serverName);
}

std::string Client::get_realname()
{
	return (this->_realName);
}

void Client::append_client_recv_buf(std::string client_recv_buf)
{
	this->_clientRecvBuf.append(client_recv_buf);
}

void Client::append_client_send_buf(std::string client_send_buf)
{
	this->_clientSendBuf.append(client_send_buf);
}

std::string Client::get_client_recv_buf()
{
	return (this->_clientRecvBuf);
}

std::string Client::get_client_send_buf()
{
	return (this->_clientSendBuf);
}

int Client::get_clientFd()
{
	return (this->_clientFd);
}

bool Client::get_pass_regist()
{
	return (this->_isRegistPass);
}

bool Client::get_nick_regist()
{
	return (this->_isRegistNick);
}

bool Client::get_user_regist()
{
	return (this->_isRegistUser);
}

void Client::set_pass_regist(bool is_regist_pass)
{
	this->_isRegistPass = is_regist_pass;
}

void Client::set_nick_regist(bool is_regist_nick)
{
	this->_isRegistNick = is_regist_nick;
}

void Client::set_user_regist(bool is_regist_user)
{
	this->_isRegistUser = is_regist_user;
}

void Client::clear_client_recv_buf()
{
	this->_clientRecvBuf.clear();
}

void Client::clear_client_send_buf()
{
	this->_clientSendBuf.clear();
}

bool	Client::get_is_regist()
{
	return (_isRegistPass && _isRegistNick && _isRegistUser);
}