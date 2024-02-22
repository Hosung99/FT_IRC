#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "main.hpp"
#include "Bot.hpp"

#define INVITE 1
#define TOPIC 2
#define KEY 4
#define LIMIT 8

class Channel
{
private:
	Bot *_bot;
	std::vector<int> _operatorFdList;
	std::string _channelName;
	std::vector<int> _clientFdList;
	std::vector<int> _inviteFdList;
	std::string _topic;
	unsigned char _mode;
	unsigned int _limit;
public:
	Channel(const std::string &channelName, int fd);
	~Channel();
	void setChannelName(std::string &channelName);
	void appendClientFdList(int clientFd);
	void removeClientFdList(int clientFd);
	std::string getChannelName();
	std::vector<int> getClientFdList();
	std::vector<int>::iterator findMyClientIt(int fd);
	bool checkClientInChannel(int fd);

	// operator
	std::vector<int> getOperatorFdList();
	void addOperatorFd(int fd);
	void removeOperatorFd(int fd);
	bool checkOperator(int fd);

	// mode
	void setMode(unsigned char mode, char sign);
	std::string getMode();
	bool checkMode(unsigned char mode);

	// limit
	void setLimit(unsigned int limit);
	unsigned int getLimit();

	// topic
	void setTopic(std::string topic);
	std::string getTopic();

	// invite
	void appendInviteFdList(int fd);
	bool checkInvite(int fd);

	Bot *getBot();
};

#endif