#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "../main.hpp"
# include "../includes/Bot.hpp"

# define INVITE	1
# define TOPIC	2
# define KEY	4
# define LIMIT	8

class Channel
{
public:
	/* OCCF */
	Channel(const std::string&, int);
	~Channel();

	/* member functions */
	// getters
	std::string					getChannelName();
	std::vector<int>			getClientFdList();
	Bot*						getBot();
	std::vector<int>			getOperatorFdList();
	std::string					getMode();
	unsigned int				getLimit();
	std::string					getTopic();
	// setters
	void						setChannelName(std::string&);
	void						setMode(unsigned char, char);
	void						setLimit(unsigned int);
	void						setTopic(std::string);
	void						setKey(std::string);
	// check
	bool						checkMode(unsigned char);
	bool						checkInvite(int);
	bool						checkKey(std::string);
	bool						checkClientInChannel(int);
	bool						checkOperator(int);
	// others
	void						appendClientFdList(int);
	void						removeClientFdList(int);
	std::vector<int>::iterator	findMyClientIt(int);
	void						addOperatorFd(int);
	void						removeOperatorFd(int);
	void						appendInviteFdList(int);

private:
	Bot*						_bot;
	std::vector<int>			_operatorFdList;
	std::string					_channelName;
	std::vector<int>			_clientFdList;
	std::vector<int>			_inviteFdList;
	std::string					_topic;
	std::string					_key;
	unsigned char				_mode;
	unsigned int				_limit;
};

#endif