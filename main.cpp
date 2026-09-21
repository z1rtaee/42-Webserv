# include "Webserv.hpp"
# include <signal.h>

/*void FunctionalEnv(void);

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

int	main(int argc, char ** argv)
{
	FunctionalEnv();
	try
	{
		mota_function(); // mota automatiza isto!!
		Sockets::mainLoop();
	}
	catch(WebExceptions &e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	std::cout << "closing everything beautifully" << std::endl;
	Sockets::delEverything();
}*/

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}

	try
	{
		Configuration config(argv[1]);
		const std::vector<ServerConfig>& servers = config.getServers();

		for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it)
		{
			std::cout << *it << std::endl;

			for (std::vector<LocationConfig>::const_iterator loc_it = it->getLocations().begin(); loc_it != it->getLocations().end(); ++loc_it)
			{
				std::cout << *loc_it << std::endl;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
