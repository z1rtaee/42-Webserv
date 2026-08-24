#include "Webserv.hpp"
#include "../../inc/HTTP/Request.hpp"

std::vector<struct pollfd>	Sockets::AllSockets;
std::vector<Info *>	Sockets::SocketInfo;

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

void	Sockets::ClientRequest(int ind)
{
	ClientInfo *Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	char	Rec[BUFFER_SIZE + 1];
	int		bread;

	bread = read (AllSockets[ind].fd, Rec, BUFFER_SIZE);
	Rec[bread] = '\0';
	// Client->request.parseRequest(Rec);	
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

void	Sockets::ServerResponse(int ind)
{
	ClientInfo *Client = dynamic_cast<ClientInfo *>(SocketInfo[ind]);
	int		bwriten;

	Client->ResponseMsg = STDHTTPResponse();
	bwriten = write (AllSockets[ind].fd, Client->ResponseMsg.c_str(), BUFFER_SIZE);
	Client->ResponseMsg.erase(0, bwriten);
	Client->SentFlag = Client->ResponseMsg.empty();
	AllSockets[ind].revents = 0;
}

void Sockets::addClient(int ind)
{
	ServerInfo *Server = dynamic_cast<ServerInfo *>(SocketInfo[ind]);

	int SvAddrSize = sizeof(Server->SvAddStruct);

	int fd = accept(AllSockets[ind].fd, (sockaddr *)&(Server->SvAddStruct), (socklen_t *)&SvAddrSize);
	if (fd == -1)
		throw WebExceptions::AcceptingClientsException();

	struct pollfd New_pollfd;

	New_pollfd.events = POLLIN;
	New_pollfd.fd = fd;
	New_pollfd.revents = 0;

	SocketInfo.push_back(new ClientInfo(*(dynamic_cast<ServerInfo *>(SocketInfo[ind]))));
	AllSockets.push_back(New_pollfd);
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
