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
	info.root = "/";
	info.ABSroot = "/home/jlima-so/42/ms5/42-Webserv"; // como mudar isto de computador pra computador?
	info.name = "www.webserving.com";
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
	catch(...)
	{
		perror("Error");
	}
	std::cout << "closing everything beautifully" << std::endl;
	Sockets::delEverything();
}
