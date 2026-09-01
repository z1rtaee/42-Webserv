#include "Webserv.hpp"
#include "Core/LocalCore.hpp"

int CGI_Info::func()
{
	return (1);
}

CGI_Info::CGI_Info(ClientInfo &ClientRef):
Info(CLIENT),
ClientRef(ClientRef),
request(),
requestReceived(INCOMPLETE),
response(""),
responseSent(INCOMPLETE)
{
	
}
