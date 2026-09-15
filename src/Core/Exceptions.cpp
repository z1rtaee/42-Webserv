#include "Webserv.hpp"

const char	*WebExceptions::what() const throw()
{
	return ("Error StdException");
}

const char	*AllowSocketAddReuseException::what() const throw()
{
	return ("Error AllowSocketAddReuseException");
}

const char	*ServerSideReadException::what() const throw()
{
	return ("Error ServerSideReadException");
}

const char	*ServerSideErrorCGI::what() const throw()
{
	return ("Error ServerSideErrorCGI");
}

const char	*ForkCGIException::what() const throw()
{
	return ("Error ForkCGIException");
}

const char	*CreatingServerSocketException::what() const throw()
{
	return ("Error CreatingServerSocketException");
}

const char	*NamingSocketWithBindException::what() const throw()
{
	return ("Error NamingSocketWithBindException");
}

const char	*AcceptingClientsException::what() const throw()
{
	return ("Error AcceptingClientsException");
}

const char	*MarkingFdAsListenException::what() const throw()
{
	return ("Error MarkingFdAsListenException");
}
