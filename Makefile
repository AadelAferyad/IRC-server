NAME = IRC
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -I includes
SOURCE = srcs/main.cpp srcs/Server.cpp srcs/Client.cpp
OBJ = $(SOURCE:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
clean:
	rm -f $(OBJ)
fclean: clean
	rm -f $(NAME)
re: fclean all
.PHONY: all clean fclean re
