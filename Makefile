CPP = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98
NAME = ./ircserv
SRCS = main.cpp Server.cpp Client.cpp
HEADS = main.hpp Server.cpp Client.cpp
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
