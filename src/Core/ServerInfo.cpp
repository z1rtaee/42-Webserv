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


ServerInfo::ServerInfo(t_info ref) :
Info(SERVER),
Config(ref)
{
	SvAddStruct.sin_family = ref.domain;
	SvAddStruct.sin_addr.s_addr = htonl(ref.interface);
	SvAddStruct.sin_port = htons(ref.port);
	jj_memset((char *)SvAddStruct.sin_zero, sizeof(SvAddStruct.sin_zero));
}

ServerInfo::ServerInfo(void) :
Info(SERVER)
{
}

ServerInfo::~ServerInfo()
{
}