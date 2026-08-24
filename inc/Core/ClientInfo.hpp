#ifndef JJ_CLIENTINFO_HPP
# define JJ_CLIENTINFO_HPP

#include "Webserv.hpp"

class ServerInfo;

class ClientInfo : public Info
{
private:
	int func();
public:
	ServerInfo	&ServerRef;

	Request				request;
	enum ParseStatus	requestReceived;
	
	std::string			response;
	enum ParseStatus	responseSent;
	
	ClientInfo(ServerInfo	&inf);
};

#endif