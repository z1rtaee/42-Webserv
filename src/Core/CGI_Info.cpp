#include "Webserv.hpp"
#include <sys/wait.h>
#include <sstream>
#include <algorithm>

class ClientInfo;

static int screaming_snake_case(int i)
{
	if (i == '-' || i == ' ')
		return '_';
	return toupper(i);
}
	
int CGI_Info::func()
{
	return (1);
}

CGI_Info::CGI_Info(ClientInfo *ClientRef, struct pollfd *ClientSocket, std::string file):
Info(CGI),
ClientRef(ClientRef),
ClientSocket(ClientSocket),
file(file),
state(0),
response(),
responseStatus(INCOMPLETE)
{
}

CGI_Info::~CGI_Info(void)
{
}

template <typename T>
std::string to_str(const T &value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
};

void	CGI_Info::getCGI_env()
{
	t_info	Config = ClientRef->ServerRef->Config;

	env.push_back("SERVER_SOFTWARE=" + to_str("server/1.0.0")); // what value goes here
	env.push_back("SERVER_NAME=" + Config.name);
	env.push_back("SERVER_PROTOCOL=" + to_str("HTTP/1.1")); // this is dynamic
	env.push_back("SERVER_PORT=" + to_str(Config.port));
	env.push_back("GATEWAY_INTERFACE=" + to_str("CGI/1.1"));
	env.push_back("REQUEST_METHOD=" + to_str("GET")); // this is dynamic
	env.push_back("SCRIPT_NAME=" + Config.root + file);
	env.push_back("SCRIPT_FILENAME=" + Config.ABSroot + Config.root + file);
	// env.push_back("PATH_INFO=" + ); //!! this is dynamic path that comer after script name
	// env.push_back("QUERY_STRING=" + ); //!! this is dynamic everything that comes after the query, as is
	// env.push_back("REQUEST_URI=" + ); //!! this is dynamic, its the exact uri the client searched for
	// env.push_back("CONTENT_LENGTH=" + to_str(lengh)); //!! this is dynamic

	// if (find("x-forwarded-for") != end()) //!! idk if this is needed actually
	// 	env.push_back("REMOTE_ADDR=" + ); // !!find the argument for "x-forwarded-for" its the client IP address
	// if (find("content-type") != end()) //!! idk if this is needed actually
	// 	env.push_back("CONTENT_TYPE=" + );  //!!find the argument for "content-type" its the MIME type of the request body, in the exact form the client sent it (including parameters)
	// for (HEADER::const_iterator it = HEADER::begin(); it != HEADER::end(); it++) //!!
	// {
	// 	// MAGNIFICENTLY stolen from raquel 
	// 	// -- add "HTTP_" before all keys, the "-" become "_" and all uppercase
	// 	std::string key (*it).first;
	// 	std::transform(key.begin(), key.end(), key.begin(), screaming_snake_case);
	// 	env.push_back("HTTP_" + key + "=" + (*it).second);
	// }
}


void CGI_Info::execCGI()
{
	pid = fork();
	if (pid == -1)
		throw ForkCGIException();
	if (pid == 0) // child
	{
		dup2(pfd[1], 1);
		close (pfd[0]);
		close (pfd[1]);

		t_info		Config = ClientRef->ServerRef->Config;
		std::string	path = Config.ABSroot + Config.root + file;

		std::vector<char *> args;
		args.push_back(const_cast<char *>(path.c_str()));
		args.push_back(NULL);

		getCGI_env();
		std::vector<char *> exec_env;
		for (size_t i = 0; i < env.size(); i++)
			exec_env.push_back(const_cast<char *>(env[i].c_str()));
		exec_env.push_back(NULL);
		execve(args[0], &args[0], &exec_env[0]);
		throw ServerSideErrorCGI();
	}
	close (pfd[1]);
	state = waitpid(pid, NULL, WNOHANG);
}


