# include "Webserv.hpp"
# include <signal.h>

void TestingEnvironment(void);
static void WebCore(void);
extern int	G_STOP_VAR;

int	main(void)
{
	// TestingEnvironment();

	t_info info;

	info.domain = DOMAIN;
	info.interface = INTERFACE;
	info.port = PORT;
	info.protocol = PROTOCOL;
	info.queue = QUEUE;
	info.root = "./";
	info.type = TYPE;
	info.events = POLLIN;
// | POLLHUP | POLLERR

	try
	{
		Sockets::addServer(info);
		while (G_STOP_VAR)
		{
			switch (poll(&Sockets::AllSockets[0], Sockets::AllSockets.size(), WAIT))
			{
			case 0:
				std::cout << "no events yet" << std::endl;
				break;
			case -1:
				std::cout << "fuck ass error which I do not understand yet" << std::endl;
				G_STOP_VAR = 0;
				break;
			default:
				WebCore();
			}
		}
	}
	catch(const WebExceptions& e)
	{
		perror("Error");
		std::cerr << e.what() << '\n';
	}
	Sockets::delEverything();
	std::cout << "closing everything beautifully" << std::endl;
}

// comments stolen from raquel :P
static void WebCore(void)
{
	for (int ind = 0; ind < Sockets::AllSockets.size(); ind++)
	{
		int		fd = Sockets::AllSockets[ind].fd;
		int		revents = Sockets::AllSockets[ind].revents;

		// --- New Connection ---
		if (revents & POLLIN && Sockets::SocketInfo[ind]->type == SERVER)
		{
			std::cout << "new client" << std::endl;
			Sockets::addClient(ind);
		}

		// --- CGI ---
		// else if (_pollfds[i].revents & (POLLIN | POLLHUP | POLLERR) && CGI::isCGISocket(fd))
			// recieveCgiOutput(fd, &i);

		// --- Read (CLIENTE Sends REQUEST) ---
		else if (revents & (POLLIN))
		{
			std::cout << "message reveived" << std::endl;
			Sockets::ClientRequest(ind);
			std::cout << "message sent" << std::endl;
			Sockets::ServerResponse(ind);
			Sockets::delSocket(ind); // not every connection wants to be immediatelly closed
		}

		// --- Write (Server sends response) ---
		// else if (revents & POLLOUT)
		// {
		// }

		// --- CheckFor timeout in possible client ---
		// else if (Sockets::type[ind] == CLIENT)
			// checkTimeout()
		std::cout << "main loop" << std::endl;
	}
}
