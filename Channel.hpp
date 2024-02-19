#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "main.hpp"

class Channel
{
private:
	int _channelFd;
	std::string _channelName;
	std::vector<int> _clientFdList;

public:
	Channel(const std::string& channelName, int fd);
	~Channel();
	void setChannelFd(int channelFd);
	void setChannelName(std::string& channelName);
	void appendClientFdList(int clientFd);
	void removeClientFdList(int clientFd);
	int getChannelFd();
	std::string getChannelName();
	std::vector<int> getClientFdList();
	std::vector<int>::iterator findMyClientIt(int fd);
	bool checkClientInChannel(int fd);
};

#endif