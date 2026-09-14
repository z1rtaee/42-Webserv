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
	// CGI_Info	*CGIref;

	Request		request;
	ParseStatus	requestStatus;

	std::string	response;
	ParseStatus	responseStatus;

	ClientInfo(ServerInfo	&inf);
	~ClientInfo();
};

#endif