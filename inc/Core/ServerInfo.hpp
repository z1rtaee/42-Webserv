#ifndef JJ_SERVERINFO_HPP
# define JJ_SERVERINFO_HPP

#include "Core/structs.hpp"
#include <netinet/in.h>

class CGI_Info;
class Info;
class ClientInfo;

class ServerInfo : public Info
{
private:
	int func();
public:
	t_info		 		Config;
	struct sockaddr_in	SvAddStruct;

	ServerInfo(t_info ref);
	ServerInfo(void);
	~ServerInfo();
};

#endif