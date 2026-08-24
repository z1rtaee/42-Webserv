#include "Webserv.hpp"
#include "Core/LocalCore.hpp"

int ClientInfo::func()
{
	return (1);
}

ClientInfo::ClientInfo(ServerInfo &ServerRef):
Info(CLIENT),
ServerRef(ServerRef),
request(),
ReceivedFlag(0),
ResponseMsg(""),
SentFlag(0)
{
	
}