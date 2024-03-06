#ifndef ERROR_HPP
# define ERROR_HPP

# include "../main.hpp"
# include "./Client.hpp"

class Client;

/* nosuch error */
void	err_nosuchnick_401(Client&, std::string);
void	err_nosuchchannel_403(Client&, std::string);

/* channel error */
void	err_usernotinchannel_441(Client&, std::string, std::string);
void	err_notonchannel_442(Client&, std::string);
void	err_useronchannel_443(Client&, std::string , std::string);

/* 43* error */
void	err_nonicknamegiven_431(Client&);
void	err_erroneusnickname_432(Client&);
void	err_nicknameinuse_433(Client&);

/* 45* error */
void	err_notregistered_451(Client&);

/* 46* error */
void	err_needmoreparams_461(Client&);
void	err_alreadyregistred_462(Client&);
void	err_passwdmismatch_464(Client&);

/* mode 47* error */
void	err_channelisfull_471(Client&, std::string);
void	err_unknownmode_472(Client&, char);
void	err_inviteonlychan_473(Client&, std::string);
void	err_badchannelkey_475(Client&, std::string);

/* 48* error */
void	err_chanoprivsneeded_482(Client&, std::string);

#endif
