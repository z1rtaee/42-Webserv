# include "Webserv.hpp"
# include <signal.h>

void FunctionalEnv(void);

t_info	build_t_info(ServerConfig ref)
{
	t_info	ret;

	ret.ErrorPages = ref.getErrorPages();
	ret.Locations = ref.getLocations();
	ret.MaxBodySize = ref.getClientMaxBodySize();
	ret.root = ref.getRoot();
	ret.name = "www.webserving.com";
	return (ret);
}

int	start_web_server()
{

}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}
	FunctionalEnv();
	try
	{
		Configuration config(argv[1]);
		const std::vector<ServerConfig>& servers = config.getServers();

		for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it)
		{
			t_info	SvConfig = build_t_info(*it);
			for (std::vector<t_endpoint>::const_iterator loc_it = it->getListen().begin(); loc_it != it->getListen().end(); ++loc_it)
			{
				SvConfig.endpoint = *loc_it;
				Sockets::addServer(SvConfig);
			}
		}
		Sockets::mainLoop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
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
	return 0;
}
