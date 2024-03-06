#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "./Server.hpp"

class Client
{
public:
	/* OCCF */
	Client(int);
	~Client();

	/* member functions */
	// getters
	std::string							getNickname();
	std::string							getUsername();
	std::string							getHostname();
	std::string							getServername();
	std::string							getRealname();
	bool								getIsRegist();
	bool								getPassRegist();
	bool								getNickRegist();
	bool								getUserRegist();
	int									getClientFd();
	std::string							getClientRecvBuf();
	std::vector<std::string>			&getChannelList();
	// setters
	void								setNickname(std::string);
	void								setUser(std::string, std::string, std::string, std::string);
	void								setPassRegist(bool);
	void								setNickRegist(bool);
	void								setUserRegist(bool);
	void								setRegist(bool);
	// append
	void								appendClientRecvBuf(std::string);
	void								appendChannelList(std::string);
	// clear
	void								clearClientRecvBuf();
	void								clearChannelList();
	void								clearClient();
	// others
	void								makeClientToBot();
	void								removeChannel(std::string);
	std::vector<std::string>::iterator	findChannel(std::string);

private:
	/* OCCF*/
	Client &operator=(const Client&);

	/* member variables */
	// client informations
	std::string							_nickName;
	std::string							_userName;
	std::string							_hostName;
	std::string							_serverName;
	std::string							_realName;
	// regist flags
	bool								_isRegistPass;
	bool								_isRegistNick;
	bool								_isRegistUser;
	// others
	std::string							_clientRecvBuf;
	std::vector<std::string>			_channelList;
	int									_clientFd;
};

#endif