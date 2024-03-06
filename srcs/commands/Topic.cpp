#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::topic(int fd, std::vector<std::string> command_vec)
{
	/* TOPIC <channel> <topic-message> */
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(_server.getClients().find(fd)->second);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (channel == NULL)
	{
		err_nosuchchannel_403(_server.getClients().find(fd)->second, command_vec[1]);
		return;
	}
	std::vector<int> clientFdList = channel->getClientFdList();
	std::vector<int>::iterator iter = clientFdList.begin();
	for (; iter != clientFdList.end(); iter++)
	{
		if (*iter == fd)
		{
			break;
		}
	}
	if (iter == clientFdList.end())
	{
		err_notonchannel_442(_server.getClients().find(fd)->second, command_vec[1]);
		return;
	}
	if (channel->checkMode(TOPIC))
	{
		if (channel->checkOperator(fd) == false)
		{
			err_chanoprivsneeded_482(_server.getClients().find(fd)->second, command_vec[1]);
			return;
		}
	}
	if (command_vec.size() == 2)
	{
		if (channel == NULL)
		{
			err_nosuchchannel_403(_server.getClients().find(fd)->second, command_vec[1]);
			return;
		}
		if (channel->getTopic().length() == 0)
		{
			_server.getClients().find(fd)->second.appendClientRecvBuf("331 " + _server.getClients().find(fd)->second.getNickname() + " " + command_vec[1] + " :" + RPL_NOTOPIC);
			return;
		}
		_server.getClients().find(fd)->second.appendClientRecvBuf("332 " + _server.getClients().find(fd)->second.getNickname() + " " + command_vec[1] + " :" + channel->getTopic() + "\r\n");
	}
	else
	{
		if (command_vec[2] == ":")
		{
			channel->setTopic("");
		}
		else
		{
			std::string topic = command_vec[2];
			for (size_t i = 3; i < command_vec.size(); i++)
			{
				topic += " " + command_vec[i];
			}
			channel->setTopic(topic);
		}
		msgToAllChannel(fd, command_vec[1], "TOPIC", channel->getTopic());
	}
}
