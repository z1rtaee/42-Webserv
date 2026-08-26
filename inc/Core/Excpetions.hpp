#ifndef JJ_EXCPETIONS_HPP
# define JJ_EXCPETIONS_HPP

#include "Webserv.hpp"

class WebExceptions
{
public:
	class CreatingServerSocketException : public std::exception
	{
		public:
		const char* what() const throw();
	};
	class NamingSocketWithBindException : public std::exception
	{
		public:
		const char* what() const throw();
	};
	class AcceptingClientsException : public std::exception
	{
		public:
		const char* what() const throw();
	};
	class MarkingFdAsListenException : public std::exception
	{
		public:
		const char* what() const throw();
	};
};

#endif