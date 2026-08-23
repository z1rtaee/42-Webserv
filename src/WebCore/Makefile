NAME =		WebServer

LIB = 		WebCore.a

SRC_FILES =	$(wildcard *.cpp) \
			$(wildcard test/*.cpp)

OBJ_FILES =	$(SRC_FILES:.cpp=.o)

CXX = c++

CXXFLAGS = -g
# -Wall -Wextra -Werror -std=c++98

all: $(LIB)

#$(NAME): $(LIB) main/main.cpp
#	$(CXX) $(CXXFLAGS) main/main.cpp $(LIB) -o $@

$(LIB): $(OBJ_FILES)
	ar csr $(LIB) $?

clean:
	rm -fr $(OBJ_FILES) $(LIB)

fclean: clean
	rm -fr $(NAME)
	
re: fclean all

.PHONY: re clean fclean all