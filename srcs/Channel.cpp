#include "../includes/Channel.hpp"

Channel::Channel(const std::string &channelName, int fd)
{
	this->_channelName = channelName;
	this->_operatorFdList.push_back(fd);
	this->_bot = new Bot();
	this->_mode = 0;
}

Channel::~Channel()
{
	delete this->_bot;
}

void Channel::setChannelName(std::string &channelName)
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

std::vector<int> Channel::getOperatorFdList()
{
	return (this->_operatorFdList);
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

void Channel::addOperatorFd(int fd)
{
	if (std::find(this->_operatorFdList.begin(), this->_operatorFdList.end(), fd) == this->_operatorFdList.end())
		this->_operatorFdList.push_back(fd);
}

bool Channel::checkOperator(int fd)
{
	if (std::find(this->_operatorFdList.begin(), this->_operatorFdList.end(), fd) != this->_operatorFdList.end())
		return (true);
	return (false);
}

void Channel::removeOperatorFd(int fd)
{
	std::vector<int>::iterator it = std::find(this->_operatorFdList.begin(), this->_operatorFdList.end(), fd);
	if (it != this->_operatorFdList.end())
		this->_operatorFdList.erase(it);
}

void Channel::setMode(unsigned char mode, char sign)
{
	if (sign == '+')
		this->_mode |= mode;
	else if (sign == '-')
		this->_mode &= ~mode;
}

std::string Channel::getMode()
{
	std::string ret;
	if (this->_mode & TOPIC)
		ret += "t";
	if (this->_mode & INVITE)
		ret += "i";
	if (this->_mode & LIMIT)
		ret += "l";
	if (this->_mode & KEY)
		ret += "k";
	return (ret);
}

bool Channel::checkMode(unsigned char mode)
{
	if (this->_mode & mode)
		return (true);
	return (false);
}

void Channel::setLimit(unsigned int limit)
{
	this->_limit = limit;
}

unsigned int Channel::getLimit()
{
	return (this->_limit);
}

Bot *Channel::getBot()
{
	return (this->_bot);
}

void Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

std::string Channel::getTopic()
{
	return (this->_topic);
}

void Channel::appendInviteFdList(int fd)
{
	if (std::find(this->_inviteFdList.begin(), this->_inviteFdList.end(), fd) == this->_inviteFdList.end())
		this->_inviteFdList.push_back(fd);
}

bool Channel::checkInvite(int fd)
{
	if (std::find(this->_inviteFdList.begin(), this->_inviteFdList.end(), fd) != this->_inviteFdList.end())
		return (true);
	return (false);
}

void Channel::setKey(std::string key)
{
	this->_key = key;
}

bool Channel::checkKey(std::string key)
{
	if (this->_key == key)
		return (true);
	return (false);
}
