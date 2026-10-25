#ifndef JJ_STRUCTS_HPP
# define JJ_STRUCTS_HPP

#include "config/ServerConfig.hpp"
#include <string>

typedef struct s_server_info 
{
	t_endpoint					endpoint; // ip / ports

	size_t						MaxBodySize;//bea needs this
	std::string					Extention;	//bea needs this
	std::string					root;		//bea needs this
	std::vector<t_error_page>	ErrorPages; //bea needs this
	std::vector<LocationConfig>	Locations;	//bea needs this

	std::string					name; // stays the same
} t_info;

typedef enum _type
{
	SERVER = 1000,
	CLIENT,
	CGI
} e_type;

#endif