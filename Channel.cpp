#include "Channel.hpp"

Channel::Channel(const std::string& channelName, int fd)
{
	this->_channelName = channelName;
	this->_channelFd = fd;
}

Channel::~Channel()
{
}

void Channel::setChannelFd(int channelFd)
{
	this->_channelFd = channelFd;
}

void Channel::setChannelName(std::string& channelName)
{
	this->_channelName = channelName;
}

void Channel::appendClientFdList(int client_fd)
{
	this->_clientFdList.push_back(client_fd);
}

void Channel::removeClientFdList(int client_fd)
{
	std::vector<int>::iterator it = findMyClientIt(client_fd);
	if (it != this->_clientFdList.end())
		this->_clientFdList.erase(it);
}

int Channel::getChannelFd()
{
	return (this->_channelFd);
}

std::string Channel::getChannelName()
{
	return (this->_channelName);
}

std::vector<int> Channel::getClientFdList()
{
	return (this->_clientFdList);
}

std::vector<int>::iterator Channel::findMyClientIt(int fd)
{
	return (std::find(this->_clientFdList.begin(), this->_clientFdList.end(), fd));
}

bool Channel::checkClientInChannel(int fd)
{
	std::vector<int>::iterator it = findMyClientIt(fd);
	if (it != this->_clientFdList.end())
		return (true);
	return (false);
}
