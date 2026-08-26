#ifndef JJ_EXCPETIONS_HPP
# define JJ_EXCPETIONS_HPP

#include "Webserv.hpp"

class WebExceptions : public std::exception
{
public:
	class CreatingServerSocketException
	{
		public:
		virtual const char* what() const throw();
	};
	class NamingSocketWithBindException
	{
		public:
		virtual const char* what() const throw();
	};
	class AcceptingClientsException
	{
		public:
		virtual const char* what() const throw();
	};
	class MarkingFdAsListenException
	{
		public:
		virtual const char* what() const throw();
	};

};

#endif