#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
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

// NICK
#define ERR_NICKCOLLISION "Nickname collision KILL from"

#endif