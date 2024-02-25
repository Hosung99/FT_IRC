#include "../../includes/Command.hpp"
#include "../../includes/Util.hpp"

void Command::mode(int fd, std::vector<std::string> command_vec)
{
	// parsing
	Client *client = _server.getClients().find(fd)->second;
	if (command_vec.size() < 2)
	{
		client->appendClientRecvBuf("461 :");
		client->appendClientRecvBuf(ERR_NEEDMOREPARAMS);
		return;
	}
	Channel *channel = _server.findChannel(command_vec[1]);
	if (client->getNickname() != command_vec[1] && channel == NULL) // 채널이 없을때
	{
		client->appendClientRecvBuf("403 " + client->getNickname() + " " + command_vec[1] + " :" + ERR_NOSUCHCHANNEL);
		return;
	}
	if (channel != NULL && command_vec.size() == 2) // 채널의 모드를 보여줄때
	{
		client->appendClientRecvBuf("324 " + client->getNickname() + " " + command_vec[1] + " +" + channel->getMode() + "\r\n");
		return;
	}
	if (client->getNickname() == command_vec[1] && command_vec[2] == "+i") // 클라이언트의 입장 명령어
	{
		client->appendClientRecvBuf("324 " + client->getNickname() + " " + command_vec[1] + " +i" + "\r\n");
		return;
	}
	if (channel != NULL && !channel->checkOperator(fd)) // 채널의 오퍼레이터가 아닐때
	{
		client->appendClientRecvBuf("482 " + client->getNickname() + command_vec[1] + " :" + ERR_CHANOPRIVSNEEDED);
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
		if (mode[i] == 'i')
		{
			if (sign == '+' && channel->checkMode(INVITE))
				continue;
			if (sign == '-' && !channel->checkMode(INVITE))
				continue;
			if (client->getNickname() == command_vec[1])
			{
				client->appendClientRecvBuf("324 " + client->getNickname() + " " + command_vec[1] + " +i" + "\r\n");
				return;
			}
			channel->setMode(INVITE, sign);
			isSetMode = true;
		}
		else if (mode[i] == 't')
		{
			if (sign == '+' && channel->checkMode(TOPIC))
				continue;
			if (sign == '-' && !channel->checkMode(TOPIC))
				continue;
			channel->setMode(TOPIC, sign);
			isSetMode = true;
		}
		else if (mode[i] == 'k')
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
		else if (mode[i] == 'l')
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
		else if (mode[i] == 'o')
		{
			if (command_vec.size() <= modeArgIndex)
			{
				continue;
			}
			Client *target = _server.findClient(command_vec[modeArgIndex]);
			if (target == NULL) // 해당 클라이언트가 서버에 없을때
			{
				client->appendClientRecvBuf("401 " + client->getNickname() + " " + command_vec[modeArgIndex] + " :" + ERR_NOSUCHNICK);
				return;
			}
			else // 클라이언트가 서버에는 있음
			{
				if (client->getNickname() == target->getNickname())
				{
					return;
				}
				if (!channel->checkClientInChannel(target->getClientFd())) // 채널에는 없음
				{
					client->appendClientRecvBuf("441 " + command_vec[modeArgIndex] + " " + command_vec[1] + " :" + ERR_USERNOTINCHANNEL);
					return;
				}
				else if (sign == '+') // 오퍼레이터 추가
				{
					channel->addOperatorFd(target->getClientFd());
					isSetMode = true;
					modeArgList.push_back(command_vec[modeArgIndex]);
					modeArgIndex++;
				}
				else if (sign == '-') // 오퍼레이터 삭제
				{
					channel->removeOperatorFd(target->getClientFd());
					isSetMode = true;
					modeArgList.push_back(command_vec[modeArgIndex]);
					modeArgIndex++;
				}
			}
		}
		else
		{
			client->appendClientRecvBuf("472 " + client->getNickname() + " " + mode[i] + " :" + ERR_UNKNOWNMODE);
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
