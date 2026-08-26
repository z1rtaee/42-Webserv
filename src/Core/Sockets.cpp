#include "Webserv.hpp"
#include "../../inc/HTTP/Request.hpp"

std::vector<struct pollfd>	Sockets::AllSockets;
std::vector<Info *>			Sockets::SocketInfo;

extern int	G_STOP_VAR;

void Sockets::mainLoop(void)
{
	while (G_STOP_VAR)
	{
		switch (poll(&AllSockets[0], AllSockets.size(), WAIT))
		{
		case -1:
			std::cout << "fuck ass error which I do not understand yet" << std::endl;
			// deal_with_fuckass_error_function(); 
			G_STOP_VAR = 0;
			break;
		case 0:
			std::cout << "no events yet" << std::endl;
			break;
		default:
			WebCore();
		}
	}
}

// comments stolen from raquel :P
void Sockets::WebCore(void)
{
	try
	{
		for (int ind = 0; ind < AllSockets.size(); ind++)
		{
			int		revents = AllSockets[ind].revents;

			switch (SocketInfo[ind]->type)
			{
			case SERVER:
			// --- New Connection ---
				if (revents & POLLIN)
					addClient(ind);
			case CLIENT:
			// --- Read (Server reads requests) ---
				if (revents & POLLIN)
					ClientRequest(ind);
			// --- Write (Server writes response) ---
				if (revents & POLLOUT)
					ServerResponse(ind);
			// --- HangUP / Timeout exceeded ---
				else if (revents & (POLLHUP | POLLERR) /*|| timeOut(ind)*/)
					delSocket(ind);
			// --- CheckFor timeout in possible client ---
				// else if (Sockets::type[ind] == CLIENT)
					// checkTimeout()
			break;
				// case CGI:
				// --- CGI ---
				// else if (_pollfds[i].revents & (POLLIN | POLLHUP | POLLERR) && CGI::isCGISocket(fd))
				// recieveCgiOutput(fd, &i);
				
				// --- Read (CLIENTE Sends REQUEST) ---
				// /* code */
				// break;
			default:
				continue;
			}
			std::cout << "main loop" << std::endl;
		}
	}
	catch(...)
	{
		// deal with whatever error occurs
	}
}

int Sockets::operator[](int ind)
{
	if (ind >= AllSockets.size() || ind < 0)
		return (-1); // or throw exception? see what the guys want
	return (AllSockets[ind].fd);
}

int Sockets::find(int SocketID)
{
	for (int ind = 0; ind < AllSockets.size(); ind++)
		if (AllSockets[ind].fd == SocketID)
			return (ind);
	return (-1);
}

void	Sockets::delEverything()
{
	while (AllSockets.size())
		delSocket(0);	
}

const char *STDHTTPResponse()
{
	return (
"HTTP/1.1 201 Created\r\n\
Content-Type: application/json\r\n\
Location: http://example.com/users/123\r\n\
\r\n\
{\r\n\
  message: New user created,\r\n\
  user: {\r\n\
    id: 123,\r\n\
    firstName: Example,\r\n\
    lastName: Person,\r\n\
    email: bsmth@example.com\r\n\
  }\r\n\
}\r\n\
\r\n"
);
}

void	Sockets::ClientRequest(int ind)
{
	ClientInfo	*Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	char		Rec[BUFFER_SIZE + 1];
	int			bread;

	std::cout << "message reveived" << std::endl;
	bread = read (AllSockets[ind].fd, Rec, BUFFER_SIZE);
	// if (bread == -1)
		// handle error
	Rec[bread] = '\0';
	// Client->requestReceived = Client->request.parseRequest(Rec);
	if (Client->requestReceived == COMPLETE)
		AllSockets[ind].revents = AllSockets[ind].revents - POLLIN;
}

void	Sockets::ServerResponse(int ind)
{
	ClientInfo *Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	int		bwriten;

	std::cout << "message sent" << std::endl;
	Client->response = STDHTTPResponse();
	while (Client->responseSent == INCOMPLETE)
	{
		bwriten = BUFFER_SIZE * (BUFFER_SIZE > Client->response.size()) + Client->response.size() * (Client->response.size() > BUFFER_SIZE);
		bwriten = write (AllSockets[ind].fd, Client->response.c_str(), bwriten);
		Client->response.erase(0, bwriten);
		if (bwriten <= 0 || Client->response.empty())
			Client->responseSent = COMPLETE;
	}
	AllSockets[ind].revents = AllSockets[ind].revents - POLLOUT;
}

void Sockets::addClient(int ind)
{
	ServerInfo *Server = dynamic_cast<ServerInfo *>(SocketInfo[ind]);

	int SvAddrSize = sizeof(Server->SvAddStruct);

	int fd = accept(AllSockets[ind].fd, (sockaddr *)&(Server->SvAddStruct), (socklen_t *)&SvAddrSize);
	if (fd == -1)
		throw WebExceptions::AcceptingClientsException();

	struct pollfd New_pollfd;

	New_pollfd.events = POLLIN | POLLHUP | POLLOUT | POLLERR;
	New_pollfd.fd = fd;
	New_pollfd.revents = 0;

	SocketInfo.push_back(new ClientInfo(*(dynamic_cast<ServerInfo *>(SocketInfo[ind]))));
	AllSockets.push_back(New_pollfd);
	std::cout << "\t\t\tadded new client" << std::endl;
}

void Sockets::addServer(t_info &Config)
{
	int	fd = socket(Config.domain, Config.type, Config.protocol);

	if (fd == -1)
		throw WebExceptions::CreatingServerSocketException();

	ServerInfo *Server = new ServerInfo(Config);

	if (bind(fd, (struct sockaddr *)&(Server->SvAddStruct), sizeof(Server->SvAddStruct)) == -1)
		throw WebExceptions::NamingSocketWithBindException();

	if (listen(fd, Config.queue) == -1)
		throw WebExceptions::MarkingFdAsListenException();
	
	struct pollfd New_pollfd;

	New_pollfd.events = Config.events;
	New_pollfd.fd = fd;
	New_pollfd.revents = 0;

	AllSockets.push_back(New_pollfd);
	SocketInfo.push_back(Server);
}

Sockets::Sockets()
{
}

void	Sockets::delSocket(int ind)
{
	delete (SocketInfo[ind]);
	close (AllSockets[ind].fd);
	SocketInfo.erase(SocketInfo.begin() + ind);
	AllSockets.erase(AllSockets.begin() + ind);
}

Sockets::~Sockets()
{
}
