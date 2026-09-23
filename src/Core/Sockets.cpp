#include "Webserv.hpp"
#include "HTTP/Request.hpp"
#include <sys/wait.h>
#include <ctime>
std::vector<struct pollfd>	Sockets::AllSockets;
std::vector<Info *>			Sockets::SocketInfo;

extern int	G_STOP_VAR;

class CGI_Info;
class Info;
class ServerInfo;
class ClientInfo;

void	Sockets::mainLoop(void)
{
	while (G_STOP_VAR)
	{
		switch (poll(&AllSockets[0], AllSockets.size(), WAIT))
		{
		case -1:
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

int	Sockets::timeOut(int ind)
{
/*	time_t timeout;

	if (dynamic_cast<CGI_Info *>(SocketInfo[ind]))
		timeout = dynamic_cast<CGI_Info *>(SocketInfo[ind])->ClientRef->request.;
	else if (dynamic_cast<ClientInfo *>(SocketInfo[ind]))
		timeout = dynamic_cast<ClientInfo *>(SocketInfo[ind])->ServerRef->Config.timeout;
	time_t curr_time;

	time(&curr_time);
	return (curr_time - SocketInfo[ind]->timestamp > SocketInfo[ind].)*/
	return (0);
}

// comments stolen from raquel :P
void	Sockets::WebCore(void)
{
	try
	{
		for (int ind = 0; ind < AllSockets.size(); ind++)
		{
			int		revents = AllSockets[ind].revents;

			switch (SocketInfo[ind]->type)
			{
			case SERVER:
				std::cout << "Server, socket nbr:" << ind << std::endl;
			// --- New Connection ---
				if (revents & POLLIN)
					addClient(ind);
				if (revents & POLLHUP || revents & POLLERR)
					delSocket(ind);
				break;
			case CLIENT:
				std::cout << "CLient, socket nbr:" << ind << std::endl;
			// --- Read (Server reads requests) ---
				if (revents & POLLIN)
					ClientRequest(ind);
			// --- Write (Server writes response) ---
				else if (revents & POLLOUT)
					ServerResponse(ind);
			// --- HangUP / Timeout exceeded ---
				if (revents & POLLHUP || revents & POLLERR || timeOut(ind))
					delSocket(ind);
				break;
			case CGI:
				std::cout << "CGI, socket nbr:" << ind << std::endl;
			// --- CGI ---
				if (revents & POLLIN)
					handleCGI(ind);
			// --- HangUP / Timeout exceeded ---
				if (revents & POLLHUP || revents & POLLERR || timeOut(ind))
					delSocket(ind);
				break;
			default:
				continue;
			}
		}
	}
	catch(std::exception &e)
	{
		std::cout << e.what() << std::endl;
		G_STOP_VAR = 0;
	}
}

void	Sockets::handleCGI(int ind)
{
	CGI_Info	*cgi = dynamic_cast<CGI_Info *>(SocketInfo[ind]);
	char		str[BUFFER_SIZE + 1];
	int			bread;

	bread = read (cgi->pfd[0], str, BUFFER_SIZE);
	if (bread < 0)
		throw ServerSideReadException();
	str[bread] = '\0';
	cgi->ClientRef->response = cgi->ClientRef->response + str;
	if (cgi->state == 0)
		cgi->state = waitpid(cgi->pid, NULL, WNOHANG);
	if (cgi->state > 0 && bread < BUFFER_SIZE) // !! is this correct???????
		cgi->responseStatus = COMPLETE;
	else if (cgi->state < 0)
		throw ServerSideErrorCGI();
	if (cgi->responseStatus == COMPLETE)
	{
		cgi->ClientSocket->events = POLLOUT;
		AllSockets[ind].revents = POLLHUP;
	}
	time(&(cgi->timestamp));
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

const char *STDHTTPResponse()
{
	return (
"HTTP/1.1 201 Created\r\n\
Content-Type: application/json\r\n\
Location: http://example.com/users/123\r\n\
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

std::string STDHTTPCGIResponse(std::string &str)
{
	str = "HTTP/1.1 201 Created\r\n\
Content-Type: application/json\r\n\
Location: http://example.com/users/123\r\n" + str + "\r\n\r\n";
	return (str);
}

void	Sockets::addCGI(int ind)
{
	ClientInfo *Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	CGI_Info	*cgi;

	cgi = new CGI_Info(Client, &(AllSockets[ind]), "ls" /*, Client->request.file*/); //!! add dynamic file

	if (pipe(cgi->pfd)== -1)
	{
		delete (cgi);
		throw ServerSideErrorCGI();
	}

	struct pollfd New_pollfd;

	New_pollfd.events = POLLIN;
	New_pollfd.fd = cgi->pfd[0];
	New_pollfd.revents = 0;

	AllSockets.push_back(New_pollfd);
	SocketInfo.push_back(cgi);

	cgi->execCGI();
	time(&(cgi->timestamp));
	std::cout << "\t\t\tadded new CGI" << std::endl;
}

void	Sockets::ClientRequest(int ind)
{
	ClientInfo	*Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	char		Rec[BUFFER_SIZE + 1];
	int			bread;

	std::cout << "\t\t\tmessage reveived:" << std::endl;
	bread = read (AllSockets[ind].fd, Rec, BUFFER_SIZE);
	if (bread < 0)
		return delSocket(ind);
	Rec[bread] = '\0';
	// Client->requestStatus = Client->request.parseRequest(Rec); // uncomment this to test HTTP request
	Client->requestStatus = COMPLETE; // delete this
	std::cout << Rec << std::endl;
	std::cout << "\t\t\tI jet read " << bread << " bytes" << std::endl;
	if (Client->requestStatus == COMPLETE)
	{
		AllSockets[ind].events = POLLOUT;
		AllSockets[ind].revents = 0;
	}
	else if (Client->requestStatus == 1000)
	{
		AllSockets[ind].revents = 0;
		AllSockets[ind].events = 0;
		addCGI(ind);
	}
	time(&(Client->timestamp));
}

void	Sockets::ServerResponse(int ind)
{
	ClientInfo *Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	int		bwriten;

	if (Client->response.empty() && Client->requestStatus == COMPLETE) // substitute with COMPLETE
		Client->response = STDHTTPResponse(); // delete this and add HTTP response
	if (Client->response.empty() && Client->requestStatus == 1000) // substitute with CGI
		Client->response = STDHTTPCGIResponse(Client->response); // delete this and add HTTP CGI response
	std::cout << "\t\t\tabout to write a response"  << std::endl;
	bwriten = write (AllSockets[ind].fd, Client->response.c_str(), Client->response.size());
	if (bwriten < 0)
		return delSocket(ind);
	Client->response.erase(0, bwriten);
	if (Client->response.empty())
	{
		AllSockets[ind].events = POLLIN;
		AllSockets[ind].revents = 0;
	}
	std::cout << "\t\t\tmessage sent with " << bwriten << " characters" << std::endl;
	time(&(Client->timestamp));
}

void	Sockets::addClient(int ind)
{
	ServerInfo *Server = dynamic_cast<ServerInfo *>(SocketInfo[ind]);

	int SvAddrSize = sizeof(Server->SvAddStruct);

	int fd = accept(AllSockets[ind].fd, (sockaddr *)&(Server->SvAddStruct), (socklen_t *)&SvAddrSize);
	if (fd == -1)
		throw AcceptingClientsException();

	struct pollfd New_pollfd;

	New_pollfd.events = POLLIN;
	New_pollfd.fd = fd;
	New_pollfd.revents = 0;

	std::cout << "\t\t\tadded new client" << std::endl;

	SocketInfo.push_back(new ClientInfo((dynamic_cast<ServerInfo *>(SocketInfo[ind]))));
	AllSockets.push_back(New_pollfd);
}

void	Sockets::addServer(t_info &Config)
{
	int	fd = socket(Config.domain, Config.type, Config.protocol);

	if (fd == -1)
		throw CreatingServerSocketException();

	// adding an option so that the socket can be reused
	// SOL_SOCKET: sets the option on API level
	// SO_REUSEADDR: allows adress to be reused without cooldown
	// optval: 1 enable, 0 disable
	int optval = 1;
	
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
	{
		close (fd);
		throw AllowSocketAddReuseException();
	}

	ServerInfo *Server = new ServerInfo(Config);

	if (bind(fd, (struct sockaddr *)&(Server->SvAddStruct), sizeof(Server->SvAddStruct)) == -1)
	{
		close (fd);
		delete (Server);
		throw NamingSocketWithBindException();
	}

	if (listen(fd, Config.queue) == -1)
	{
		close (fd);
		delete (Server);
		throw MarkingFdAsListenException();
	}
	
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

void	Sockets::delEverything()
{
	while (AllSockets.size())
	{
		dellAllClients ((dynamic_cast<ServerInfo *>(SocketInfo[0])));
		delSocket(0);
	}
}

void Sockets::removeRefClient(ClientInfo *ref)
{
	if (ref->CGIref)
		ref->CGIref->ClientRef = NULL; // no dangling pointers on my watch
}

void Sockets::dellAllClients(ServerInfo *ref)
{
	for (int ind = 0; ind < SocketInfo.size(); ind++)
		while (ind < SocketInfo.size() && \
				dynamic_cast<ClientInfo *>(SocketInfo[ind]) && \
				(dynamic_cast<ClientInfo *>(SocketInfo[ind]))->ServerRef == ref)
			delSocket(ind);
}

void Sockets::removeRefCGI(CGI_Info *ref)
{
	if (ref->ClientRef)
	{
		close(ref->pfd[0]);
		ref->ClientRef->CGIref = NULL; // no dangling pointers on my watch
	}
}

void	Sockets::delSocket(int ind)
{
	std::cout << "\t\t\t\t\tdeleting " << ind + 1 << " out off " << SocketInfo.size() << std::endl;
	if (dynamic_cast<ClientInfo *>(SocketInfo[ind]))
	{
		std::cout << "\t\t\t\t\tdeleting client with socket: " << AllSockets[ind].fd << std::endl;
		removeRefClient ((dynamic_cast<ClientInfo *>(SocketInfo[ind])));
	}
	else if (dynamic_cast<ServerInfo *>(SocketInfo[ind]))
	{
		std::cout << "\t\t\t\t\tdeleting server with socket: " << AllSockets[ind].fd << std::endl;
		dellAllClients ((dynamic_cast<ServerInfo *>(SocketInfo[ind])));
	}
	else if (dynamic_cast<CGI_Info *>(SocketInfo[ind]))
	{
		std::cout << "\t\t\t\t\tdeleting CGI with socket: " << AllSockets[ind].fd << std::endl;
		removeRefCGI ((dynamic_cast<CGI_Info *>(SocketInfo[ind])));
	}

	delete (SocketInfo[ind]);
	close (AllSockets[ind].fd);
	SocketInfo.erase(SocketInfo.begin() + ind);
	AllSockets.erase(AllSockets.begin() + ind);
	std::cout << "\t\t\t\t\tdeleted... now there are " << SocketInfo.size() << std::endl;
}

Sockets::~Sockets()
{
}
