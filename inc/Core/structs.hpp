#ifndef JJ_STRUCTS_HPP
# define JJ_STRUCTS_HPP

#include "config/ServerConfig.hpp"
#include <string>

typedef struct s_server_info 
{
	t_endpoint					endpoint; // ip / ports
	std::string					root;
	size_t						MaxBodySize;
	std::vector<t_error_page>	ErrorPages; 
	std::vector<LocationConfig>	Locations;
	std::string					name; // stays the same
} t_info;

typedef enum _type
{
	SERVER = 1000,
	CLIENT,
	CGI
} e_type;

#endif