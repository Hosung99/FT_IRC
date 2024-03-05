NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98
RM			= rm -f

# files
INCS_DIR	= ./includes/
INCS		= Server.hpp \
			  Command.hpp \
			  Channel.hpp \
			  Client.hpp \
			  Bot.hpp \
			  Util.hpp \
			  Error.hpp
HEADS		= $(addprefix $(INCS_DIR), $(INCS))

SRCS_DIR	= ./srcs/
SRCS		= Server.cpp \
			  Command.cpp \
			  Channel.cpp \
			  Client.cpp \
			  Bot.cpp \
			  Util.cpp \
			  Error.cpp
OBJS_SRCS	= $(addprefix $(SRCS_DIR), $(SRCS:.cpp=.o))

CMDS_DIR	= ./srcs/commands/
CMDS		= Invite.cpp \
			  Join.cpp \
			  Kick.cpp \
			  Mode.cpp \
			  Nick.cpp \
			  Part.cpp \
			  Pass.cpp \
			  Ping.cpp \
			  Privmsg.cpp \
			  Quit.cpp \
			  Topic.cpp \
			  User.cpp
OBJS_CMDS	= $(addprefix $(CMDS_DIR), $(CMDS:.cpp=.o))

OBJS 		= main.o $(OBJS_SRCS) $(OBJS_CMDS)


# rules
all		: $(NAME)

%.o		: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME)	: $(OBJS) $(HEADS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(GREEN) "⚡︎	[ ircserv ]	Ready to run ircserv" $(RESET)

clean	:
	@$(RM) $(OBJS)
	@echo $(GREEN) "⚡︎	[ ircserv ]	Removed Object files" $(RESET)

fclean	: clean
	@$(RM) $(NAME)
	@echo $(GREEN) "⚡︎	[ ircserv ]	Removed ircserv" $(RESET)

re		:
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY: all clean fclean re

# colors
RESET	= "\x1b[0m"
GREY	= "\x1b[30m"
RED		= "\x1b[31m"
GREEN	= "\x1b[32m"
YELLOW	= "\x1b[33m"
BLUE	= "\x1b[34m"
PURPLE	= "\x1b[35m"
CYAN	= "\x1b[36m"
WHITE	= "\x1b[37m"
