### COLOURS ###
PINK 	= \e[1;35m
PURPLE	= \e[0;95m
WHITE   = \e[1;37m
DEFAULT = \e[0m

### VARS ###
NAME			= Webserv

INCLUDE			= -I inc/
CXX				= c++
CXXFLAGS		= -std=c++98 -g $(INCLUDE)

### COMPLETE TWINS LIBS ###
LIB_HTTP_PATH	= inc/libs/
LIB_HTTP		= $(LIB_HTTP_PATH)libHTTP.a
LIB_CORE_PATH	= inc/libs/
LIB_CORE		= $(LIB_CORE_PATH)libCore.a
LIB_WEBSERV		= WebServ.a

### Webserv SRCS ###
SRC_PATH		= src/
SRCS	 		= $(CONFIG_SRCS) $(HTTP_SRCS) $(CORE_SRCS)

CONFIG_PATH 	= src/config/
CONFIG_SRCS		= 
#???????????
CONFIG			= $(addprefix $(CONFIG_PATH), $(CONFIG_SRCS))

HTTP_PATH		= src/HTTP/
HTTP_SRCS		= clean.cpp
HTTP			= $(addprefix $(HTTP_PATH), $(HTTP_SRCS))

CORE_PATH		= src/Core/
CORE_SRCS		= ClientInfo.cpp Excpetions.cpp Info.cpp ServerInfo.cpp Sockets.cpp aux.cpp TestingEnvironment.cpp
CORE			= $(addprefix $(CORE_PATH), $(CORE_SRCS))

TOTAL_SRCS		= $(words $(SRCS))
FILES			= 0

### OBJECTS ###
HTTP_OBJS_PATH		= obj/
HTTP_OBJS 			= $(SRCS:%.cpp=$(HTTP_OBJS_PATH)%.o)
# CORE_OBJS 			= $(CORE:%.cpp=$(HTTP_OBJS_PATH)%.o)

vpath %.cpp $(SRC_PATH) $(CONFIG_PATH) $(HTTP_PATH) $(CORE_PATH)

### RULES ###
all: 			$(NAME)

$(HTTP_OBJS_PATH)%.o: %.cpp | $(HTTP_OBJS_PATH)
				@mkdir -p $(dir $@)
				@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@ && \
				$(eval FILES=$(shell echo $$(($(FILES) + 1)))) \
				$(call PRINT_PROGRESS, $(TOTAL_SRCS),$(PINK), $(WHITE)Compiling$(DEFAULT) $@)

$(HTTP_OBJS_PATH):
				@mkdir -p $@

$(NAME): 		$(LIB_WEBSERV)
				@echo "$(WHITE)Bringing $(PINK)$(NAME)$(WHITE) to life!$(DEFAULT)"
				@echo "$(WHITE)Creating $(PINK)$(NAME)'s$(WHITE) executable...$(DEFAULT)"
				$(CXX) $(CXXFLAGS) main.cpp $(LIB_WEBSERV) -o $@
				@echo "$(PINK)$(NAME) was born!$(DEFAULT)"

echo:
				echo $(LIB_HTTP)"\n"
				echo $(LIB_CORE)
				echo $(CORE_SRCS)

#### CRIAR ARQUIVO DIRETORIA PARA OS .A ###

$(LIB_WEBSERV): $(HTTP_OBJS) $(CORE_OBJS)
				ar -rcs $(LIB_WEBSERV) $?

# $(LIB_HTTP): 	$(HTTP_OBJS)
# 				ar -rcs $(LIB_HTTP) $?
# 
# $(LIB_CORE): 	$(CORE_OBJS)
# 				ar -rcs $(LIB_CORE) $?

### CLEAN RULES ###
clean:
				@rm -rf $(HTTP_OBJS)
				@rm -rf $(CORE_OBJS)
				@rm -rf obj
				@echo "$(WHITE)✩°｡⋆ $(PINK)Files Cleaned$(WHITE) ⋆｡°✩ \n \
	(\ (\    \n \
( ~_•)    \n \
(っ▄︻▇〓▄︻┻┳═ ─ ҉.  \n \
 /　 )  / \    \n \
(/￣∪  /   \ $(DEFAULT)"

fclean: 		clean
				@rm -f $(NAME)
				@rm -f $(LIB_WEBSERV)
				@echo "$(WHITE)⋆˚࿔ Executable Killed˚⋆♡$(DEFAULT)"

re: 			fclean all



### TIRAR
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
