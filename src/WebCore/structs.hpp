#ifndef JJ_STRUCTS_HPP
# define JJ_STRUCTS_HPP

#include "local_core.hpp"

typedef struct s_server_info 
{
	std::string	root;
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
	SERVER,
	CLIENT
} e_type;

#endif