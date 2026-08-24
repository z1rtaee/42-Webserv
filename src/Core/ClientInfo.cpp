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
requestReceived(INCOMPLETE),
response(""),
responseSent(INCOMPLETE)
{
	
}