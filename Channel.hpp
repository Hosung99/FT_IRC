#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "main.hpp"
#include "Bot.hpp"

class Channel
{
private:
	int _operatorFd;
	Bot *_bot;
	std::string _channelName;
	std::vector<int> _clientFdList;
public:
	Channel(const std::string &channelName, int fd);
	~Channel();
	void setChannelName(std::string &channelName);
	void appendClientFdList(int clientFd);
	void removeClientFdList(int clientFd);
	int getOperatorFd();
	std::string getChannelName();
	std::vector<int> getClientFdList();
	std::vector<int>::iterator findMyClientIt(int fd);
	bool checkClientInChannel(int fd);
	void setOperator(int fd);
	Bot *getBot();
};

#endif