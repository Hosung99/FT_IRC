#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <vector>
#include <utility>
#include <sstream>

#define BUF_SIZE 1024
// PASS
#define ERR_NEEDMOREPARAMS "Not enough parameters"
#define ERR_PASSWDMISMATCH "Password incorrect"
#define ERR_ALREADYREGIST "You may not reregister"

// NICK
#define ERR_NICKNAMEINUSE "Nickname is already in use"
#define ERR_NICKCOLLISION "Nickname collision KILL from"
#define ERR_NONICKNAMEGIVEN "No nickname given"
#define ERR_ERRONEUSNICKNAME "Erroneus nickname"

// common
#define ERR_NOTREGISTERED "You have not registered"

#endif