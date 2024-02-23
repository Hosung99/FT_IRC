#include "Server.hpp"
#include "Client.hpp"

unsigned short int Server::setPortNum(char *str)
{
	int ret;
	int str_index = 0;
	while (str[str_index])
	{
		if (!(str[str_index] >= '0' || str[str_index] <= '9'))
			throw(std::out_of_range("ERROR:: Port Number is only number"));
		str_index++;
	}
	ret = atoi(str);
	if (!(ret >= 1024 && ret <= 65535))
		throw(std::logic_error("ERROR:: PORT number range is 1024~65535"));
	return ((unsigned short int)ret);
}

std::string Server::setPassword(char *str)
{
	int str_index = 0;
	while (str[str_index])
		str_index++;
	if (str_index >= 9)
		throw(std::logic_error("ERROR:: Password is under 9 digit"));
	return (str);
}

Server::Server(char *portNum, char *password)
{
	this->_portNum = setPortNum(portNum);
	this->_password = setPassword(password);
	_fdCnt = 1;
	_clientAddrSize = sizeof(this->_clientAddr);
	this->_command = new Command(*this);
}

Server::~Server()
{
	close(_serverSock);
	std::map<int, Client *>::iterator iter = _clients.begin();
	for (; iter != _clients.end(); iter++)
	{
		close(iter->first);
		delete iter->second;
	}
	std::map<std::string, Channel *>::iterator iter2 = _channelList.begin();
	for (; iter2 != _channelList.end(); iter2++)
	{
		delete iter2->second;
	}
	_clients.clear();
	_channelList.clear();
	delete _command;
	delete _bot;
}

void Server::setBot()
{
	_bot = new Client(-1);
	_bot->makeClientToBot();
	_clients.insert(std::make_pair(-1, _bot));
}

void Server::run()
{
	setServerSock();
	setServerAddr();
	setServerBind();
	setServerListen();
	setBot();
	memset(_fds, 0, sizeof(_fds));
	_fds[0].fd = _serverSock;
	_fds[0].events = POLLIN;
	execute();
}

void Server::setServerSock()
{
	int option = 1;
	_serverSock = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(_serverSock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (_serverSock == -1)
		throw(std::logic_error("ERROR :: socket() error"));
}

void Server::setServerAddr()
{
	memset(&this->_serverAddr, 0, sizeof(_serverAddr));
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverAddr.sin_port = htons(this->_portNum);
}

void Server::setServerBind()
{
	if (bind(_serverSock, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
		throw(std::logic_error("ERROR:: bind() error"));
}

void Server::setServerListen()
{
	if (listen(_serverSock, 15) == -1)
		throw std::logic_error("ERROR:: listen() error");
}

int Server::getServerSock()
{
	return (this->_serverSock);
}

std::map<int, Client *> Server::getClients()
{
	return (this->_clients);
}

std::string Server::getPassword()
{
	return (this->_password);
}

std::string Server::getMessage(int fd)
{
	return (this->_message[fd]);
}

std::map<std::string, Channel *> Server::getChannelList()
{
	return (this->_channelList);
}

void Server::appendNewChannel(std::string& channelName, int fd)
{
	_channelList.insert(std::make_pair(channelName, new Channel(channelName, fd)));
}

int Server::recvMessage(int fd)
{
	char buf[2];
	ssize_t read_len;
	int ret = 0;
	while ((read_len = recv(fd, buf, 1, 0)) == 1)
	{
		ret++;
		_message[fd] += buf[0];
		if (buf[0] == '\n')
			break;
	}
	return (ret);
}

bool Server::checkMessageEnds(int fd)
{
	if (_message[fd].length() == 1 && _message[fd][0] == '\n')
		_message[fd] = "";
	if (_message[fd][_message[fd].length() - 1] == '\n' && _message[fd][_message[fd].length() - 2] == '\r')
		return (true);
	return (false);
}

void Server::doCommand(int fd)
{
	_command->run(fd);
}

void Server::addClient(int fd)
{
	_clients.insert(std::make_pair(fd, new Client(fd)));
}

void Server::execute()
{
	while (1)
	{
		int ret_poll = poll(_fds, _fdCnt, -1);
		if (ret_poll > 0)
		{
			for (int i = 0; i < _fdCnt; i++)
			{
				if (_fds[i].revents == 0)
					continue;
				if (_fds[i].fd == _serverSock)
				{
					_clientSock = accept(_serverSock, (struct sockaddr *)&_clientAddr, &_clientAddrSize);
					fcntl(_clientSock, F_SETFL, O_NONBLOCK);
					_fds[_fdCnt].fd = _clientSock;
					_fds[_fdCnt].events = POLLIN;
					_fdCnt++;
					addClient(_clientSock);
					std::cout << "fd[" << _clientSock << "] is connected" << std::endl;
				}
				else
				{
					if (_fds[i].revents & POLLIN)
					{
						_strLen = recvMessage(_fds[i].fd);
						if (_strLen <= 0)
						{
							std::cout << "fd " << _fds[i].fd << " is quit connect" << std::endl;
							Client* client = _clients.find(_fds[i].fd)->second;
							client->clearClientRecvBuf();
							client->setRegist(false);
							_clients.erase(_fds[i].fd);
							close(_fds[i].fd);
							delete client;
						}
						else
						{
							if (checkMessageEnds(_fds[i].fd))
							{
								std::cout << "fd[" << _fds[i].fd << "]: " << _message[_fds[i].fd];
								doCommand(_fds[i].fd);
								_message[_fds[i].fd] = "";
							}
						}
					}
				}
			}
			std::map<int, Client *>::iterator iter = _clients.begin();
			for (; iter != _clients.end(); iter++)
			{
				if (iter->second->getClientRecvBuf().length() > 0)
				{
					send(iter->first, iter->second->getClientRecvBuf().c_str(), iter->second->getClientRecvBuf().length(), 0);
					iter->second->clearClientRecvBuf();
				}
			}
		}
		else if (ret_poll < 0)
		{
			throw(std::logic_error("ERROR:: poll() error"));
		}
	}
}

Channel *Server::findChannel(std::string channel_name)
{
	std::map<std::string, Channel *>::iterator iter = _channelList.find(channel_name);
	if (iter == _channelList.end())
		return (NULL);
	return (iter->second);
}

Client *Server::findClient(std::string nickname)
{
	std::map<int, Client *>::iterator iter = _clients.begin();
	for (; iter != _clients.end(); iter++)
	{
		if (iter->second->getNickname() == nickname)
			return (iter->second);
	}
	return (NULL);
}

void	Server::removeChannel(std::string channel_name)
{
	_channelList.erase(channel_name);
}