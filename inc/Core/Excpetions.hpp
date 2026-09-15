#ifndef JJ_EXCPETIONS_HPP
# define JJ_EXCPETIONS_HPP

#include "Webserv.hpp"

class WebExceptions : public std::exception
{
public:
	virtual const char* what() const throw();
};

class ServerSideReadException : public WebExceptions
{
public:
	const char* what() const throw();
};

class ServerSideErrorCGI : public WebExceptions
{
public:
	const char* what() const throw();
};

class ForkCGIException : public WebExceptions
{
public:
	const char* what() const throw();
};

class AllowSocketAddReuseException : public WebExceptions
{
public:
	const char* what() const throw();
};

class CreatingServerSocketException : public WebExceptions
{
public:
	const char* what() const throw();
};

class NamingSocketWithBindException : public WebExceptions
{
public:
	const char* what() const throw();
};

class AcceptingClientsException : public WebExceptions
{
public:
	const char* what() const throw();
};

class MarkingFdAsListenException : public WebExceptions
{
public:
	const char* what() const throw();
};

#endif