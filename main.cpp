# include "Webserv.hpp"
# include <signal.h>

void TestingEnvironment(void);

void mota_function(void)
{
	t_info info;

	info.domain = DOMAIN;
	info.interface = INTERFACE;
	info.port = PORT;
	info.protocol = PROTOCOL; // provavelmente n vais mexer nisto
	info.queue = QUEUE;
	info.root = "./";
	info.type = TYPE; // provavelmente n vais mexer nisto
	info.events = POLLIN; // provavelmente n vais mexer nisto
	Sockets::addServer(info);
}

int	main(/*int argc, char ** argv*/)
{
	// TestingEnvironment();
	try
	{
		mota_function(); // mota automatiza isto!!
		Sockets::mainLoop();
	}
	catch(const WebExceptions::NamingSocketWithBindException& e)
	{
		perror("Error");
		std::cerr << e.what() << '\n';
		exit(0); // this cant be here
	}
	catch(const WebExceptions& e)
	{
		perror("Error");
		std::cerr << e.what() << '\n';
	}
	Sockets::delEverything();
	std::cout << "closing everything beautifully" << std::endl;
}
