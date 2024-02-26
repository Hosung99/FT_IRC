CPP = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g
NAME = ircserv
SRCS = main.cpp srcs/Bot.cpp srcs/Channel.cpp srcs/Client.cpp srcs/Command.cpp srcs/Server.cpp srcs/Util.cpp srcs/commands/Invite.cpp srcs/commands/Join.cpp srcs/commands/Kick.cpp srcs/commands/Mode.cpp srcs/commands/Nick.cpp srcs/commands/Part.cpp srcs/commands/Pass.cpp srcs/commands/Ping.cpp srcs/commands/Privmsg.cpp srcs/commands/Quit.cpp srcs/commands/Topic.cpp srcs/commands/User.cpp srcs/Error.cpp
HEADS = main.hpp includes/Bot.hpp includes/Channel.hpp includes/Client.hpp includes/Command.hpp includes/Server.hpp includes/Util.hpp includes/Error.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS) $(HEADS)
	$(CPP) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(OBJS)
	rm -f $(NAME)

re:
	$(MAKE) fclean
	$(MAKE) all

.PHONY: all clean fclean re
