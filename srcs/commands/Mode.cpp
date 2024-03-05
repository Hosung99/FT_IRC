#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::mode(int fd, std::vector<std::string> command_vec)
{
	/* MODE <channel> <+/- i, t, k, l, o> (<mode-needed-value>) */
	Client &client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 2)
	{
		err_needmoreparams_461(client);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (client.getNickname() != command_vec[1] && channel == NULL)	// channel not exists
	{
		err_nosuchchannel_403(client, command_vec[1]);
		return;
	}
	if (channel != NULL && command_vec.size() == 2)	// show channel-mode
	{
		client.appendClientRecvBuf("324 " + client.getNickname() + " " + command_vec[1] + " +" + channel->getMode() + "\r\n");
		return;
	}
	if (client.getNickname() == command_vec[1] && command_vec[2] == "+i")	// if not channel but user: ignore and send messages same as irssi
	{
		client.appendClientRecvBuf("324 " + client.getNickname() + " " + command_vec[1] + " +i" + "\r\n");
		return;
	}
	if (channel != NULL && !channel->checkOperator(fd))	// if not channel-operator
	{
		err_chanoprivsneeded_482(client, command_vec[1]);
		return;
	}
	std::string mode = command_vec[2];
	char sign;
	int index = 0;
	while (mode[index] == '+' || mode[index] == '-')
	{
		sign = mode[index];
		index++;
	}
	if (mode.length() == 1)
	{
		return;
	}
	std::string msg = "";
	std::vector<std::string> modeArgList;
	unsigned int modeArgIndex = 3;
	for (size_t i = index; i < mode.length(); i++)
	{
		if (mode[i] == '+' || mode[i] == '-')
			break;
		bool isSetMode = false;
		if (mode[i] == 'i')		// invite
		{
			if (sign == '+' && channel->checkMode(INVITE))
				continue;
			if (sign == '-' && !channel->checkMode(INVITE))
				continue;
			if (client.getNickname() == command_vec[1])
			{
				client.appendClientRecvBuf("324 " + client.getNickname() + " " + command_vec[1] + " +i" + "\r\n");
				return;
			}
			channel->setMode(INVITE, sign);
			isSetMode = true;
		}
		else if (mode[i] == 't')	// topic
		{
			if (sign == '+' && channel->checkMode(TOPIC))
				continue;
			if (sign == '-' && !channel->checkMode(TOPIC))
				continue;
			channel->setMode(TOPIC, sign);
			isSetMode = true;
		}
		else if (mode[i] == 'k')	// key
		{
			if (sign == '-' && !channel->checkMode(KEY))
				continue;
			if (command_vec.size() > modeArgIndex)
			{
				channel->setMode(KEY, sign);
				if (sign == '+')
				{
					channel->setKey(command_vec[3]);
				}
				else if (sign == '-')
				{
					channel->setKey("");
				}
				isSetMode = true;
				modeArgList.push_back(command_vec[modeArgIndex]);
				modeArgIndex++;
			}
		}
		else if (mode[i] == 'l')	// limit
		{
			if (sign == '-' && !channel->checkMode(LIMIT))
				continue;
			if (command_vec.size() > modeArgIndex)
			{
				std::string limit_s = command_vec[modeArgIndex].c_str();
				bool isDigit = true;
				for (size_t j = 0; j < limit_s.length(); ++j)
				{
					if (!isdigit(limit_s[j]))
					{
						isDigit = false;
						break;
					}
				}
				if (!isDigit)
				{
					modeArgIndex++;
					continue;
				}
				int limit = atoi(limit_s.c_str());
				if (limit < 0)
				{
					modeArgIndex++;
					continue;
				}
				channel->setMode(LIMIT, sign);
				if (sign == '+')
				{
					channel->setLimit(limit);
				}
				isSetMode = true;
				modeArgList.push_back(command_vec[modeArgIndex]);
				modeArgIndex++;
			}
		}
		else if (mode[i] == 'o')	// operator
		{
			if (command_vec.size() <= modeArgIndex)
			{
				continue;
			}
			std::map<int, Client>::iterator target = _server.findClient(command_vec[modeArgIndex]);
			if (target == _server.getClients().end())	// clients not exists
			{
				err_nosuchnick_401(client, command_vec[modeArgIndex]);
				return;
			}
			else	// clients exists
			{
				if (client.getNickname() == target->second.getNickname())
				{
					return;
				}
				if (!channel->checkClientInChannel(target->second.getClientFd()))	// clients not exist in channel
				{
					err_usernotinchannel_441(client, command_vec[modeArgIndex], command_vec[1]);
					return;
				}
				else if (sign == '+') // give operator-authority
				{
					channel->addOperatorFd(target->second.getClientFd());
					isSetMode = true;
					modeArgList.push_back(command_vec[modeArgIndex]);
					modeArgIndex++;
				}
				else if (sign == '-') // remove operator-authority
				{
					channel->removeOperatorFd(target->second.getClientFd());
					isSetMode = true;
					modeArgList.push_back(command_vec[modeArgIndex]);
					modeArgIndex++;
				}
			}
		}
		else	// unknown mode: error
		{
			err_unknownmode_472(client, mode[i]);
			continue;
		}
		if (isSetMode)
		{
			if (msg.empty())
				msg += sign;
			msg += mode[i];
		}
	}
	for (size_t i = 0; i < modeArgList.size(); ++i)
	{
		msg += " " + modeArgList[i];
	}
	if (msg.empty())
		return;
	msgToAllChannel(fd, command_vec[1], "MODE", msg);
}
