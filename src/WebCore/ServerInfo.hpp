#ifndef JJ_SERVERINFO_HPP
# define JJ_SERVERINFO_HPP

#include "local_core.hpp"

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
	ServerInfo &operator=(ServerInfo &ref);
};

#endif