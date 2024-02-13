CPP = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = ./ircserv
SRCS = main.cpp Server.cpp Client.cpp Command.cpp Channel.cpp
HEADS = main.hpp Server.hpp Client.hpp Command.hpp Channel.hpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o : %.cpp
	$(CPP) $(CFLAGS) -c $^ -o $@

$(NAME): $(OBJS) $(HEADS)
	$(CPP) -o $@ $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(OBJS)
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
