#ifndef JJ_CGI_INFO_HPP
# define JJ_CGI_INFO_HPP

#include "Webserv.hpp"

class ServerInfo;

class CGI_Info : public Info
{
private:
	int func();
public:
	ClientInfo	&ClientRef;
	int			pfd[2];

	std::string	Exec;

	e_CGIState	state;

	std::string	response;
	ParseStatus	responseStatus;

	CGI_Info(ClientInfo	&inf, std::string file);
	~CGI_Info(void);

	void execCGI();
};

#endif