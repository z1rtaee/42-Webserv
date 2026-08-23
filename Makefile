### COLOURS ###
PINK 	= \e[1;35m
PURPLE	= \e[0;95m
WHITE   = \e[1;37m
DEFAULT = \e[0m

### VARS ###
NAME			= webserv

CXX				= c++
CXXFLAGS		= -Wall -Wextra -Werror -std=c++98 -g
INCLUDE			= -I inc/

### WEBSERV SRCS ###
SRCS	 		= $(CONFIG_SRCS) $(HTTP_SRCS) main.cpp HTTP/clean.cpp
SRC_PATH		= src/

CONFIG_SRCS		= 
CONFIG_PATH 	= src/config/
CONFIG			= $(addprefix $(CONFIG_PATH), $(CONFIG_SRCS))

HTTP_SRCS		= 
HTTP_PATH		= src/HTTP/
HTTP			= $(addprefix $(HTTP_PATH), $(HTTP_SRCS))

TOTAL_SRCS		= $(words $(SRCS))
FILES			= 0

### OBJECTS ###
OBJS_PATH		= obj/
OBJS 			= $(SRCS:%.cpp=$(OBJS_PATH)%.o)
ALL_OBJ			= $(OBJS_PATH)*.o

vpath %.cpp $(SRC_PATH) $(CONFIG_PATH) $(HTTP_PATH)

### RULES ###
all: 			$(NAME)

$(OBJS_PATH)%.o: %.cpp | $(OBJS_PATH)
				@mkdir -p $(dir $@)
				@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ && \
				$(eval FILES=$(shell echo $$(($(FILES) + 1)))) \
				$(call PRINT_PROGRESS, $(TOTAL_SRCS),$(PINK), $(WHITE)Compiling$(DEFAULT) $@)

$(OBJS_PATH):
				@mkdir -p $@

$(NAME): 		$(OBJS) $(OBJS_PATH)
				@echo "$(WHITE)Bringing $(PINK)$(NAME)$(WHITE) to life!$(DEFAULT)"
				@echo "$(WHITE)Creating $(PINK)$(NAME)'s$(WHITE) executable...$(DEFAULT)"
				@$(CXX) $(CXXFLAGS) $(OBJS) -o $@
				@echo "$(PINK)$(NAME) was born!$(DEFAULT)"

### CLEAN RULES ###
clean:
				@rm -rf $(OBJS)
				@rm -rf obj
				@echo "$(WHITE)✩°｡⋆ $(PINK)Files Cleaned$(WHITE) ⋆｡°✩ \n \
	(\ (\    \n \
( ~_•)    \n \
(っ▄︻▇〓▄︻┻┳═ ─ ҉.  \n \
 /　 )  / \    \n \
(/￣∪  /   \ $(DEFAULT)"

fclean: 		clean
				@rm -f $(NAME)
				@echo "$(WHITE)⋆˚࿔ Executable Killed˚⋆♡$(DEFAULT)"

re: 			fclean all

v:				$(NAME)
				clear && valgrind --show-leak-kinds=all --leak-check=full --track-fds=all --track-origins=yes --suppressions=readline.supp ./minishell

ch:				$(NAME)
				valgrind --show-leak-kinds=all --leak-check=full --track-fds=all --track-origins=yes --suppressions=readline.supp --trace-children=yes ./minishell

.PHONY: 		all clean fclean re v ch


### PRINTED PROGRESS BAR ###
define PRINT_PROGRESS
    if [ "$(FILES)" -eq "1" ]; then \
        printf "\033[0K$(3)\n["; \
    else \
        printf "\033[0K\033[1F\033[0K$(3)\n["; \
    fi
    @for i in `seq 1 $(shell expr $(FILES) \* 70 / $(1))`; do \
        printf "$(2)=\033[0m"; \
    done
    @for i in `seq 1 $(shell expr 70 - $(FILES) \* 70 / $(1))`; do \
        printf " "; \
    done
    @printf "] $(shell echo $$(($(FILES) * 100 / $(1))))%%"
	if [ "$(FILES)" -eq "$(1)" ]; then \
        printf "\n"; \
	fi	
endef
