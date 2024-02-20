#ifndef MAIN_HPP
#define MAIN_HPP

#include "Util.hpp"
#include <algorithm>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include <utility>
#include <string.h>

#define BUF_SIZE 1024
// PASS
#define ERR_NEEDMOREPARAMS "Not enough parameters\r\n"
#define ERR_PASSWDMISMATCH "Password incorrect\r\n"
#define ERR_ALREADYREGIST "You may not reregister\r\n"

// NICK
#define ERR_NICKNAMEINUSE "Nickname is already in use\r\n"
#define ERR_NICKCOLLISION "Nickname collision KILL from\r\n"
#define ERR_NONICKNAMEGIVEN "No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME "Erroneus nickname\r\n"

// PRIVMSG
#define ERR_NOSUCHCHANNEL "No such channel\r\n"
#define ERR_NOSUCHNICK "No such nick/channel\r\n"

// COMMON
#define ERR_NOTREGISTERED "You have not registered\r\n"

#endif