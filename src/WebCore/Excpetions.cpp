#include "local_core.hpp"

const char	*WebExceptions::CreatingServerSocketException::what() const throw()
{
	return ("Error creating server socket");
}

const char	*WebExceptions::NamingSocketWithBindException::what() const throw()
{
	return ("Error naming socket with bind");
}

const char	*WebExceptions::AcceptingClientsException::what() const throw()
{
	return ("Error accepting clients");
}

const char	*WebExceptions::MarkingFdAsListenException::what() const throw()
{
	return ("Error Marking Fd As Listen");
}
