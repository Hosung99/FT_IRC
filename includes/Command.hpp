#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "../main.hpp"
# include "./Client.hpp"
# include "./Channel.hpp"
# include "./Server.hpp"
# include "./Error.hpp"

class Client;
class Server;
class Channel;

class Command
{
public:
	/* OCCF */
	Command(Server &server);
	~Command();

	/* member functions */
	// verify commands
	void		run(int);
	// commands/
	void		pass(int, std::vector<std::string>);
	void		nick(int, std::vector<std::string>);
	void		user(int, std::vector<std::string>);
	void		ping(int, std::vector<std::string>);
	void		privmsg(int, std::vector<std::string>);
	void		quit(int, std::vector<std::string>);
	void		part(int, std::vector<std::string>);
	void		join(int, std::vector<std::string>);
	void		kick(int, std::vector<std::string>);
	void		mode(int, std::vector<std::string>);
	void		topic(int, std::vector<std::string>);
	void		invite(int, std::vector<std::string>);
	// utils in commands/
	void		botCommand(int, std::vector<std::string>);
	void		topicMsg(int, std::string);
	bool		checkNicknameDuplicate(std::string, std::map<int, Client>&);
	bool		checkNicknameValidate(std::string);
	bool		checkRealname(std::string);
	bool		checkBotCommand(std::string);
	// utils in Util.cpp
	std::string	channelMessage(int, std::vector<std::string>);
	void		channelPRIVMSG(std::string, Client&, Channel*);
	void		channelPART(int, std::string, std::vector<std::string>);
	void		msgToAllChannel(int, std::string, std::string, std::string);
	std::string	makeFullName(int);
	void		nameListMsg(int, std::string);

private:
	/* OCCF */
	Command &operator=(const Command&);
	Command(const Command&);

	/* member variables */
	Server		&_server;
};

#endif