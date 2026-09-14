#ifndef JJ_STRUCTS_HPP
# define JJ_STRUCTS_HPP

#include <string>

typedef struct s_server_info 
{
	std::string	root;
	std::string	ABSroot;
	std::string	RELroot;
	std::string	name;
	int			domain;
	int			type;
	int			protocol;
	int			interface;
	int			port;
	int			queue;
	int			events;
} t_info;

typedef enum _type
{
	SERVER = 1000,
	CLIENT,
	CGI
} e_type;

#endif