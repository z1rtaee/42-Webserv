#ifndef JJ_CLIENTINFO_HPP
# define JJ_CLIENTINFO_HPP

#include <string>
#include "HTTP/Request.hpp"
#include "Core/Info.hpp"
#include "Core/CGI_Info.hpp"
#include "Core/ServerInfo.hpp"

class CGI_Info;
class Info;
class ServerInfo;

class ClientInfo : public Info
{
private:
	int func();
public:
	ServerInfo	*ServerRef;
	CGI_Info	*CGIref;

	Request		request; 
	ParseStatus	requestStatus;

	std::string	response;
	ParseStatus	responseStatus;

	ClientInfo(ServerInfo *inf);
	~ClientInfo();
};

#endif