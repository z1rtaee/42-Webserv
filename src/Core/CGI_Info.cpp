#include "Webserv.hpp"
#include "Core/LocalCore.hpp"
#include <sys/wait.h>
#include <sstream>

class ClientInfo;
	
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
/*
std::vector<std::string> CGI_Info::getCGI_env(std::string RELpath)
{
	ServerInfo	*ServerRef = ClientRef->ServerRef;
	const std::string _method_names[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "PATCH", "UNSUPPORTED_METHOD"};
	const std::string _protocol_names[] = {"HTTP/1.0", "HTTP/1.1", "UNSUPPORTED_PROTOCOL"};

	env.push_back("SERVER_SOFTWARE=" + to_str("server/1.0.0"));
	env.push_back("SERVER_NAME=" + ServerRef->Config.name);
	env.push_back("SERVER_PROTOCOL=" + to_str("HTTP/1.1")); // this is dynamic
	env.push_back("SERVER_PORT=" + to_str(ServerRef->Config.port));
	env.push_back("GATEWAY_INTERFACE=" + to_str("CGI/1.1"));

	env.push_back("REQUEST_METHOD=" + to_str("GET")); // this is dynamic
	// -- path info is whatever comes after the program name in the url
	// env.push_back("PATH_INFO=" + req.path_uri); // this is dynamic
	// env.push_back("QUERY_STRING=" + req.query); // this is dynamic
	env.push_back("SCRIPT_NAME=" + RELpath);
	env.push_back("SCRIPT_FILENAME=" + ServerRef->Config.ABSroot + RELpath);

	// if (find("x-forwarded-for"))
		// env.push_back("REMOTE_ADDR=" + find the argument for "x-forwarded-for");
	env.push_back("REQUEST_URI=" + req.path_uri + req.query); 
	// env.push_back("CONTENT_LENGTH=" + to_str(_body.size())); // this is dynamic
	// if (req.headers.find("content-type") != req.headers.end())
		// env.push_back("CONTENT_TYPE=" + req.headers["content-type"]);

	// for (map_strings::const_iterator it = req.headers.begin(); it != req.headers.end(); it++)
	// {
		// -- add "HTTP_" before all keys, the "-" become "_" and all uppercase
		// std::string key = (*it).first;
		// std::transform(key.begin(), key.end(), key.begin(), ::screaming_snake_case);
		// env.push_back("HTTP_" + key + "=" + (*it).second);
	// } perguntar a raquel o que caralhos isto faz?????
	std::vector<char *> ServerEnv = ClientRef->ServerRef->getCGI_env();
	std::vector<char *> ServerEnv = ClientRef->getCGI_env();
}
*/

void CGI_Info::execCGI()
{
	/*
	pid = fork();
	if (pid == -1)
		throw WebExceptions::AcceptingClientsException(); // !! wrong exception
	if (pid == 0) // child
	{
		dup2(pfd[1], 1);
		close (pfd[0]);
		close (pfd[1]);

		std::string path = ClientRef->ServerRef->Config.root + file;

		std::vector<char *> args;
		args.push_back(const_cast<char *>(path.c_str()));
		args.push_back(NULL);
		env = getCGI_env(path);
		for (size_t i = 0; i < env.size(); i++)
			exec_env.push_back(const_cast<char *>(env[i].c_str()));
		exec_env.push_back(NULL);
		execve(args[0], &args[0], &_env[0]);
	}
	close (pfd[1]);
	state = waitpid(pid, NULL, WNOHANG);
	*/
}


