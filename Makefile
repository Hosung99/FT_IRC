CPP = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g
NAME = ./ircserv
SRCS = main.cpp Server.cpp Client.cpp Command.cpp Util.cpp Channel.cpp Bot.cpp
HEADS = main.hpp Server.hpp Client.hpp Command.hpp Util.hpp Channel.hpp Bot.hpp
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

re: fclean all

.PHONY: all clean fclean re
