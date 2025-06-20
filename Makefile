C++ = c++
STD = -std=c++23
C++_FLAGS = -Wall -Wextra -Werror
INFILES = src/Server.cpp
INCLUDES = -Iincludes -I/opt/homebrew/opt/readline/include

OBJFILES = $(INFILES:.cpp=.o)

NAME = ircserv

all: $(NAME)

$(NAME):$(OBJFILES)
	$(C++) $(STD) $(C++_FLAGS) $(OBJFILES) -o $(NAME)

%.o: %.cpp
	$(C++) $(STD) $(C++_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJFILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re