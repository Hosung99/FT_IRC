#include "Channel.hpp"

Channel::Channel(const std::string& channel_name, int fd)
{
	this->_channel_name = channel_name;
	this->_channel_fd = fd;
}

Channel::~Channel()
{
}

void Channel::set_channel_fd(int channel_fd)
{
	this->_channel_fd = channel_fd;
}

void Channel::set_channel_name(std::string& channel_name)
{
	this->_channel_name = channel_name;
}

void Channel::append_client_fd_list(int client_fd)
{
	this->_client_fd_list.push_back(client_fd);
}

void Channel::remove_client_fd_list(int client_fd)
{
	std::vector<int>::iterator it = std::find(this->_client_fd_list.begin(), this->_client_fd_list.end(), client_fd);
	if (it != this->_client_fd_list.end())
		this->_client_fd_list.erase(it);
}

int Channel::get_channel_fd()
{
	return (this->_channel_fd);
}

std::string Channel::get_channel_name()
{
	return (this->_channel_name);
}

std::vector<int> Channel::get_client_fd_list()
{
	return (this->_client_fd_list);
}

std::vector<int>::iterator Channel::findMyClientIt(int fd)
{
	return (std::find(this->_client_fd_list.begin(), this->_client_fd_list.end(), fd));
}

bool Channel::checkClientInChannel(int fd)
{
	std::vector<int>::iterator it = findMyClientIt(fd);
	if (it != this->_client_fd_list.end())
		return (true);
	return (false);
}
