#ifndef ERROR_HPP
# define ERROR_HPP

# include "../main.hpp"
#include "Client.hpp"

class Client;

// nosuch error
void err_nosuchnick_401(Client &client, std::string nickname);
void err_nosuchchannel_403(Client &client, std::string channel);

// channel error
void err_notonchannel_442(Client &client, std::string channel);
void err_useronchannel_443(Client &client, std::string nickname, std::string channel);
void err_usernotinchannel_441(Client &client, std::string nickname, std::string channel);

// 45* error
void err_notregistered_451(Client &client);

// 46* error
void err_needmoreparams_461(Client &client);
void err_alreadyregistred_462(Client &client);
void err_passwdmismatch_464(Client &client);

// 43* error
void err_nonicknamegiven_431(Client &client);
void err_erroneusnickname_432(Client &client);
void err_nicknameinuse_433(Client &client);

// mode 47* error
void err_channelisfull_471(Client &client, std::string channel);
void err_unknownmode_472(Client &client, char mode);
void err_inviteonlychan_473(Client &client, std::string channel);
void err_badchannelkey_475(Client &client, std::string channel);

void err_chanoprivsneeded_482(Client &client, std::string channel);

#endif
