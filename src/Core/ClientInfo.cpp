#include "Webserv.hpp"

int ClientInfo::func()
{
	return (1);
}

ClientInfo::ClientInfo(ServerInfo *ServerRef):
Info(CLIENT),
ServerRef(ServerRef),
CGIref(NULL),
request(ServerRef->Config),
requestStatus(INCOMPLETE),
response(),
responseStatus(INCOMPLETE)
{
}

ClientInfo::~ClientInfo()
{
}

// char** CGI_Info::createEnvFromHTTP()
// {
// 
// }
