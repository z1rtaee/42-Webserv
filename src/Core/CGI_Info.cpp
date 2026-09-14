#include "Webserv.hpp"
#include "Core/LocalCore.hpp"

class ClientInfo;
	
int CGI_Info::func()
{
	return (1);
}

CGI_Info::CGI_Info(ClientInfo &ClientRef, std::string file):
Info(CGI),
ClientRef(ClientRef),
Exec(file),
state(READING),
response(),
responseStatus(INCOMPLETE)
{
}

CGI_Info::~CGI_Info(void)
{
}

void CGI_Info::execCGI()
{
	int pid = fork();
	if (pid == -1)
		throw WebExceptions::AcceptingClientsException(); // !! wrong exception
	if (pid == 0)
	{
		close (pfd[0]);
		dup2(pfd[1], 1);
		// char **env = createEnvFromHTTP();
	}

	state = READING;
}