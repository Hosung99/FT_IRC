#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::invite(int fd, std::vector<std::string> command_vec)
{
	/* INVITE <nickname> <channel> */
	Client &client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 3)
	{
		err_needmoreparams_461(client);
		return;
	}
	std::map<int, Client>::iterator target = _server.findClient(command_vec[1]);
	if (target == _server.getClients().end())
	{
		err_nosuchnick_401(client, command_vec[1]);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[2]);
	if (channel == NULL)
	{
		err_nosuchchannel_403(client, command_vec[2]);
		return;
	}
	if (!channel->checkClientInChannel(client.getClientFd()))
	{
		err_notonchannel_442(client, command_vec[2]);
		return;
	}
	if (!channel->checkOperator(client.getClientFd()))
	{
		err_chanoprivsneeded_482(client, command_vec[2]);
		return;
	}
	if (channel->checkClientInChannel(target->second.getClientFd()))
	{
		err_useronchannel_443(client, command_vec[1], command_vec[2]);
		return;
	}
	target->second.appendClientRecvBuf(":" + client.getNickname() + " INVITE " + target->second.getNickname() + " " + command_vec[2] + "\r\n");
	client.appendClientRecvBuf("341 " + client.getNickname() + " " + command_vec[1] + " " + command_vec[2] + " :" + RPL_INVITING);
	channel->appendInviteFdList(target->second.getClientFd());
}
