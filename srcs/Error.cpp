#include "../includes/Error.hpp"

/* nosuch error */
void err_nosuchnick_401(Client &client, std::string nickname)
{
	client.appendClientRecvBuf("401 " + client.getNickname() + " " + nickname + " :" + ERR_NOSUCHNICK);
}

void err_nosuchchannel_403(Client &client, std::string channel)
{
	client.appendClientRecvBuf("403 " + client.getNickname() + " " + channel + " :" + ERR_NOSUCHCHANNEL);
}

/* channel error */
void err_usernotinchannel_441(Client &client, std::string nickname, std::string channel)
{
	client.appendClientRecvBuf("441 " + client.getNickname() + " " + nickname + " " + channel + " :" + ERR_USERNOTINCHANNEL);
}

void err_notonchannel_442(Client &client, std::string channel)
{
	client.appendClientRecvBuf("442 " + client.getNickname() + " " + channel + " :" + ERR_NOTONCHANNEL);
}

void err_useronchannel_443(Client &client, std::string nickname, std::string channel)
{
	client.appendClientRecvBuf("443 " + client.getNickname() + " " + nickname + " " + channel + " :" + ERR_USERONCHANNEL);
}

/* 43* error */
void err_nonicknamegiven_431(Client &client)
{
	client.appendClientRecvBuf("431 :");
	client.appendClientRecvBuf(ERR_NONICKNAMEGIVEN);
}

void err_erroneusnickname_432(Client &client)
{
	client.appendClientRecvBuf("432 :");
	client.appendClientRecvBuf(ERR_ERRONEUSNICKNAME);
}

void err_nicknameinuse_433(Client &client)
{
	client.appendClientRecvBuf("433 :");
	client.appendClientRecvBuf(ERR_NICKNAMEINUSE);
}

/* 45* error */
void err_notregistered_451(Client &client)
{
	client.appendClientRecvBuf("451 :");
	client.appendClientRecvBuf(ERR_NOTREGISTERED);
}

/* 46* error */
void err_needmoreparams_461(Client &client)
{
	client.appendClientRecvBuf("461 :");
	client.appendClientRecvBuf(ERR_NEEDMOREPARAMS);
}

void err_alreadyregistred_462(Client &client)
{
	client.appendClientRecvBuf("462 :");
	client.appendClientRecvBuf(ERR_ALREADYREGISTRED);
}

void err_passwdmismatch_464(Client &client)
{
	client.appendClientRecvBuf("464 :");
	client.appendClientRecvBuf(ERR_PASSWDMISMATCH);
}

/* mode 47* error */
void err_channelisfull_471(Client &client, std::string channel)
{
	client.appendClientRecvBuf("471 " + client.getNickname() + " " + channel + " :" + ERR_CHANNELISFULL);
}

void err_unknownmode_472(Client &client, char mode)
{
	client.appendClientRecvBuf("472 " + client.getNickname() + " " + mode + " :" + ERR_UNKNOWNMODE);
}

void err_inviteonlychan_473(Client &client, std::string channel)
{
	client.appendClientRecvBuf("473 " + client.getNickname() + " " + channel + " :" + ERR_INVITEONLYCHAN);
}

void err_badchannelkey_475(Client &client, std::string channel)
{
	client.appendClientRecvBuf("475 " + client.getNickname() + " " + channel + " :" + ERR_BADCHANNELKEY);
}

/* 48* error */
void err_chanoprivsneeded_482(Client &client, std::string channel)
{
	client.appendClientRecvBuf("482 " + client.getNickname() + " " + channel + " :" + ERR_CHANOPRIVSNEEDED);
}
