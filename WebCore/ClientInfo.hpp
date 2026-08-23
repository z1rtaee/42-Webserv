#ifndef JJ_CLIENTINFO_HPP
# define JJ_CLIENTINFO_HPP

#include "local_core.hpp"

class ServerInfo;

class ClientInfo : public Info
{
private:
	int func();
public:
	ServerInfo	&ServerRef;

	std::string	RequestMsg;
	int			ReceivedFlag;
	
	std::string	ResponseMsg;
	int			SentFlag;
	
	ClientInfo(ServerInfo	&inf);
};

#endif