#ifndef JJ_EXCPETIONS_HPP
# define JJ_EXCPETIONS_HPP

#include "Webserv.hpp"

class WebExceptions : public std::exception
{
public:
	class CreatingServerSocketException
	{
		virtual const char* what() const throw();
	};
	class NamingSocketWithBindException
	{
		virtual const char* what() const throw();
	};
	class AcceptingClientsException
	{
		virtual const char* what() const throw();
	};
	class MarkingFdAsListenException
	{
		virtual const char* what() const throw();
	};

};

#endif