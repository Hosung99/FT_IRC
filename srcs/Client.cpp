#include "../includes/Client.hpp"

/* constructor */
Client::Client(int clientfd)
{
	this->_clientFd = clientfd;
	_nickName = "Client";
	_isRegistPass = false;
	_isRegistNick = false;
	_isRegistUser = false;
}

/* destructor */
Client::~Client()
{
}


/* getters */
std::string Client::getNickname()
{
	return (this->_nickName);
}

std::string Client::getUsername()
{
	return (this->_userName);
}

std::string Client::getHostname()
{
	return (this->_hostName);
}

std::string Client::getServername()
{
	return (this->_serverName);
}

std::string Client::getRealname()
{
	return (this->_realName);
}

std::string Client::getClientRecvBuf()
{
	return (this->_clientRecvBuf);
}

int Client::getClientFd()
{
	return (this->_clientFd);
}

bool Client::getPassRegist()
{
	return (this->_isRegistPass);
}

bool Client::getNickRegist()
{
	return (this->_isRegistNick);
}

bool Client::getUserRegist()
{
	return (this->_isRegistUser);
}

bool Client::getIsRegist()
{
	return (_isRegistPass && _isRegistNick && _isRegistUser);
}


/* setters */
void Client::setNickname(std::string nickname)
{
	this->_nickName = nickname;
}

void Client::setUser(std::string username, std::string hostname, std::string servername, std::string realname)
{
	this->_userName = username;
	this->_hostName = hostname;
	this->_serverName = servername;
	this->_realName = realname;
}

void Client::setPassRegist(bool is_regist_pass)
{
	this->_isRegistPass = is_regist_pass;
}

void Client::setNickRegist(bool is_regist_nick)
{
	this->_isRegistNick = is_regist_nick;
}

void Client::setUserRegist(bool is_regist_user)
{
	this->_isRegistUser = is_regist_user;
}

std::vector<std::string>& Client::getChannelList()
{
	return (this->_channelList);
}

void Client::setRegist(bool isRegist)
{
	this->_isRegistPass = isRegist;
	this->_isRegistNick = isRegist;
	this->_isRegistUser = isRegist;
}


/* append */
void Client::appendClientRecvBuf(std::string client_recv_buf)
{
	this->_clientRecvBuf.append(client_recv_buf);
}

void Client::appendChannelList(std::string channelName)
{
	this->_channelList.push_back(channelName);
}


/* clear */
void Client::clearClientRecvBuf()
{
	this->_clientRecvBuf.clear();
}

void Client::clearChannelList()
{
	this->_channelList.clear();
}

void Client::clearClient()
{
	_nickName = "";
	_userName = "";
	_hostName = "";
	_serverName = "";
	_realName = "";
	_clientFd = -2;
	this->clearChannelList();
	this->clearClientRecvBuf();
	this->setRegist(false);
}


/* others */
void Client::makeClientToBot()
{
	_nickName = "Bot";
	_userName = "Bot";
	_hostName = "Bot";
	_serverName = "Bot";
	_realName = "Bot";
	this->_isRegistPass = true;
	this->_isRegistNick = true;
	this->_isRegistUser = true;
}

void Client::removeChannel(std::string channelName)
{
	std::vector<std::string>::iterator iter = findChannel(channelName);
	if (iter != _channelList.end())
		_channelList.erase(iter);
}

std::vector<std::string>::iterator Client::findChannel(std::string channelName)
{
	std::vector<std::string>::iterator iter = _channelList.begin();
	for (; iter != _channelList.end(); iter++)
	{
		if (*iter == channelName)
			return (iter);
	}
	return (iter);
}
