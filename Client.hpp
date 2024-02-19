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
		std::string _clientSendBuf; // client가 보낼 데이터
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
		void set_nickname(std::string);
		void set_user(std::string username, std::string hostname, std::string servername, std::string realname);
		std::string get_nickname();
		std::string get_username();
		std::string get_hostname();
		std::string get_servername();
		std::string get_realname();
		void append_client_recv_buf(std::string);
		void append_client_send_buf(std::string);
		void clear_client_recv_buf();
		void clear_client_send_buf();
		std::string get_client_recv_buf();
		std::string get_client_send_buf();
		bool get_is_regist();
		bool get_pass_regist();
		bool get_nick_regist();
		bool get_user_regist();
		void set_pass_regist(bool);
		void set_nick_regist(bool);
		void set_user_regist(bool);
		int get_clientFd();

		std::vector<std::string> &getChannelList();
		void appendChannelList(std::string channelName);
		void removeChannel(std::string channelName);
		void clearChannelList();
		std::vector<std::string>::iterator findChannel(std::string channelName);

};

#endif