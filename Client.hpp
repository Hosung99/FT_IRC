#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
private:
	/* OCCF*/

	/* member variables */
	std::string _nickName;
	std::string _userName;
	std::string _hostName;
	std::string _serverName;
	std::string _realName;

	std::string _clientRecvBuf; // client가 받은 데이터
	std::vector<std::string> _channelList;
	int _clientFd;
	bool _isRegistPass;
	bool _isRegistNick;
	bool _isRegistUser;

public:
	/* OCCF */
	Client(int clientfd);
	~Client();

	/* member functions */
	void setNickname(std::string);
	void setUser(std::string username, std::string hostname, std::string servername, std::string realname);
	std::string getNickname();
	std::string getUsername();
	std::string getHostname();
	std::string getServername();
	std::string getRealname();
	void appendClientRecvBuf(std::string);
	void clearClientRecvBuf();
	std::string getClientRecvBuf();
	bool getIsRegist();
	bool getPassRegist();
	bool getNickRegist();
	bool getUserRegist();
	void setPassRegist(bool);
	void setNickRegist(bool);
	void setUserRegist(bool);
	int getClientFd();

	std::vector<std::string> &getChannelList();
	void appendChannelList(std::string channelName);
	void removeChannel(std::string channelName);
	void clearChannelList();
	std::vector<std::string>::iterator findChannel(std::string channelName);
	void makeClientToBot();
};

#endif