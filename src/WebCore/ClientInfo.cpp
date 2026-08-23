#include "local_core.hpp"

int ClientInfo::func()
{
	return (1);
}

ClientInfo::ClientInfo(ServerInfo &ServerRef):
Info(CLIENT),
request(),
ServerRef(ServerRef),
ReceivedFlag(0),
SentFlag(0)
{
	
}