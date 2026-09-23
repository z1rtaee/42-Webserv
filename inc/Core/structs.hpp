#ifndef JJ_STRUCTS_HPP
# define JJ_STRUCTS_HPP

#include <string>

typedef struct s_server_info 
{
	std::string	root; //location list
	std::string	ABSroot; //root
	std::string	name; // stays the same
	int			domain; // stays the same
	int			type; // stays the same
	int			protocol; // stays the same
	int			interface;  // t_endpoint.ip
	int			port; // t_endpoint.port
	int			queue; // stays the same
	int			events; // stays the same
} t_info;

typedef enum _type
{
	SERVER = 1000,
	CLIENT,
	CGI
} e_type;

#endif