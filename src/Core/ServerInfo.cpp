#include "Webserv.hpp"
#include "Core/LocalCore.hpp"

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

ServerInfo &ServerInfo::operator=(ServerInfo &ref)
{
	return (*this);
}

ServerInfo::~ServerInfo()
{
}