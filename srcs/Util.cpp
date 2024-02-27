#include "../includes/Util.hpp"
#include "../includes/Command.hpp"

std::vector<std::string> split(std::string str, char delimiter)
{
	std::vector<std::string> internal;
	std::stringstream ss(str);
	std::string tok;

	while (getline(ss, tok, delimiter))
	{
		internal.push_back(tok);
	}
	return (internal);
}

bool isSpecial(char c)
{
	if (c == '[' || c == ']' || c == '{' || c == '}' || c == '\\' || c == '|' || c == '_')
		return (true);
	return (false);
}

std::string Command::channelMessage(int index, std::vector<std::string> command_vec)
{
	std::string message = "";
	for (size_t i = index; i < command_vec.size(); i++)
	{
		message += command_vec[i];
		if (i != command_vec.size() - 1)
			message += " ";
	}
	if (message[0] == ':')
		message = message.substr(1, message.length() - 1);
	if (message.length() == 0)
		message = "NO REASON";
	return (message);
}

void Command::channelPRIVMSG(std::string message, Client &client, Channel *channel)
{
	std::vector<int> fdList = channel->getClientFdList();
	std::vector<int>::iterator fd_iter = fdList.begin();
	for (; fd_iter != fdList.end(); fd_iter++)
	{
		if (*fd_iter != client.getClientFd())
		{
			Client& target = _server.getClients().find(*fd_iter)->second;
			target.appendClientRecvBuf(":" + client.getNickname() + " PRIVMSG " + channel->getChannelName() + " :" + message + "\r\n");
		}
	}
}

void Command::channelPART(int fd, std::string channelName, std::vector<std::string> command_vec)
{
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator client_iter = clients.find(fd);
	Channel *channel = _server.findChannel(channelName);
	std::vector<int> fdList = channel->getClientFdList();
	std::vector<int>::iterator fd_iter = fdList.begin();
	std::string message = channelMessage(1, command_vec);
	for (; fd_iter != fdList.end(); fd_iter++)
	{
		if (*fd_iter != fd)
		{
			Client &target = _server.getClients().find(*fd_iter)->second;
			target.appendClientRecvBuf(":" + target.getNickname() + "!" + target.getUsername() + "@" + target.getServername() + client_iter->second.getNickname() + " PART " + channel->getChannelName() + " " + message + "\r\n");
		}
	}
}


void Command::msgToAllChannel(int target, std::string channelName, std::string command, std::string msg)
{
	std::map<std::string, Channel *>& channelList = _server.getChannelList();
	if (channelList.find(channelName) == channelList.end())
	{
		return;
	}
	Channel *channel = channelList.find(channelName)->second;
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	while (iter != clientFdList.end())
	{
		Client &client = _server.getClients().find(*iter)->second;
		if (command == "PRIVMSG" && target == *iter)
		{
			iter++;
			continue ;
		}
		client.appendClientRecvBuf(makeFullName(target) + " " + command + " " + channelName + " " + msg + "\r\n");
		iter++;
	}
}

std::string Command::makeFullName(int fd)
{
	std::map<int, Client>& clients = _server.getClients();
	std::map<int, Client>::iterator clientIt = clients.find(fd);
	Client &client = clientIt->second;
	std::string temp = (":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getServername());
	return temp;
}


void Command::nameListMsg(int fd, std::string channelName)
{
	std::map<std::string, Channel *>& channelList = _server.getChannelList();
	if (channelList.find(channelName) == channelList.end())
		return;
	Channel *channel = channelList.find(channelName)->second;
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	std::string message;
	while (iter != clientFdList.end())
	{
		Client &client = _server.getClients().find(*iter)->second;
		if (channel->checkOperator(client.getClientFd()))
			message += "@";
		message += client.getNickname();
		if (iter != clientFdList.end() - 1)
			message += " ";
		iter++;
	}
	Client &client = _server.getClients().find(fd)->second;
	client.appendClientRecvBuf("353 " + client.getNickname() + " = " + channelName + " :" + message + "\r\n");
	client.appendClientRecvBuf("366 " + client.getNickname() + " " + channelName + " :End of NAMES list.\r\n");
}
