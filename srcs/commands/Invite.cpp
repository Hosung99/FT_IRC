#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::invite(int fd, std::vector<std::string> command_vec)
{
	// INVITE <nickname> <channel>
	Client *client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 3)
	{
		client->appendClientRecvBuf("461 :");
		client->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Client *target = _server.findClient(command_vec[1]);
	if (target == NULL)
	{
		client->appendClientRecvBuf("401 " + client->getNickname() + " " + command_vec[1] + " :" + ERR_NOSUCHNICK);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[2]);
	if (channel == NULL)
	{
		client->appendClientRecvBuf("403 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (!channel->checkClientInChannel(client->getClientFd()))
	{
		client->appendClientRecvBuf("442 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_NOTONCHANNEL);
		return;
	}
	if (!channel->checkOperator(client->getClientFd()))
	{
		client->appendClientRecvBuf("482 " + client->getNickname() + " " + command_vec[2] + " :" + ERR_CHANOPRIVSNEEDED);
		return;
	}
	if (channel->checkClientInChannel(target->getClientFd()))
	{
		client->appendClientRecvBuf("443 " + client->getNickname() + " " + command_vec[1] + " " + command_vec[2] + " :" + ERR_USERONCHANNEL);
		return;
	}
	target->appendClientRecvBuf(":" + client->getNickname() + " INVITE " + target->getNickname() + " " + command_vec[2] + "\r\n");
	client->appendClientRecvBuf("341 " + client->getNickname() + " " + command_vec[1] + " " + command_vec[2] + " :" + RPL_INVITING);
	channel->appendInviteFdList(target->getClientFd());
}
