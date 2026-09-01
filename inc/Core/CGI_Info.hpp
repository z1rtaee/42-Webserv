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

	Request				request;
	ParseStatus	requestReceived;
	
	std::string			response;
	ParseStatus	responseSent;
	
	CGI_Info(ClientInfo	&inf);
	~CGI_Info();
};

#endif