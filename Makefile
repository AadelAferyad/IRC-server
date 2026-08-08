NAME = IRC
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I includes
SOURCE =	srcs/main.cpp srcs/Server.cpp srcs/Client.cpp srcs/Dispatcher.cpp srcs/Channel.cpp srcs/Replies.cpp\
			srcs/handlers/Pass.cpp \
			srcs/handlers/Nick.cpp \
			srcs/handlers/User.cpp \
			srcs/handlers/Join.cpp \

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
