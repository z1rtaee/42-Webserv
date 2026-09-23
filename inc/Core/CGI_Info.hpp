#ifndef JJ_CGI_INFO_HPP
# define JJ_CGI_INFO_HPP

#include "HTTP/Request.hpp"
#include "Core/Info.hpp"
#include "Core/ClientInfo.hpp"
#include "Core/structs.hpp"
#include "Core/Info.hpp"
#include "string"

class Info;
class ServerInfo;
class ClientInfo;

class CGI_Info : public Info
{
private:
	int func();
	std::vector<std::string>	env;
public:
	ClientInfo		*ClientRef;
	struct pollfd	*ClientSocket;
	int				pfd[2];
	int				pid;
	int				state;

	std::string	file;


	std::string	response;
	ParseStatus	responseStatus;

	void	getCGI_env();
	CGI_Info(ClientInfo *ClientRef, struct pollfd *ClientSocket, std::string file);
	~CGI_Info(void);

	void execCGI();
};

#endif