#include "Webserv.hpp"

void	jj_memset(char *str, int size)
{
	for (int ind = 0; ind < size; ind++)
		str[ind] = 0;
}

int ServerInfo::func()
{
	return (1);
}

#include <iostream>
#include <sstream>

int	ft_STOI(std::string str)
{
	std::stringstream	stream;
	int					sep;
	int					ret = 0;

	stream.clear();
	stream.str("");


	stream << str;
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	str = str.find(".") + 1;
	stream << str.find(".");
	stream >> sep;
	ret = (ret << 8) + sep;

	return (ret);
}

ServerInfo::ServerInfo(t_info ref) :
Info(SERVER),
Config(ref)
{
	SvAddStruct.sin_family = DOMAIN;
	SvAddStruct.sin_addr.s_addr = htonl(2130706433); //	127.0.0.1
	// SvAddStruct.sin_addr.s_addr = htonl(ft_STOI(ref./endpoint.ip));
	SvAddStruct.sin_port = htons(ref.endpoint.port);
	jj_memset((char *)SvAddStruct.sin_zero, sizeof(SvAddStruct.sin_zero));
}

ServerInfo::ServerInfo(void) :
Info(SERVER)
{
}

ServerInfo::~ServerInfo()
{
}