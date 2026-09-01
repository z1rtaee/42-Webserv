#ifndef JJ_SOCKETS_HPP
# define JJ_SOCKETS_HPP

#include "Webserv.hpp"

class Sockets
{
public:
	int	operator[](int ind);
	Sockets(); // adds Socket and related information to the list of know Sockets
	~Sockets();

	static void mainLoop(void); // main poll loop function
	static void	addServer(t_info &Config); // adds the ServerSocketID from the list of ServerSockets, does nothing if it doesnt exist
	static void	delEverything(); // deletes the SocketID from the list of sockets, does nothing if it doesnt exist

private:
	static std::vector<struct pollfd>	AllSockets; // Vector with all Sockets to be monitored by the poll() loop.
	static std::vector<Info *>			SocketInfo; // Vector with all info related the sockets

	static void ClientRequest(int ind);
	static void ServerResponse(int ind);
	static void	delSocket(int ind); // deletes the SocketID from the list of sockets, does nothing if it doesnt exist
	static void WebCore(void); // main logic behind the server
	static int	find(int SocketID); // returns the index of the SocketID on success, returns -1 on error
	static void	addClient(int ind); // adds the ClientSocketID from the list of ClientSockets, does nothing if it doesnt exist
	static void	handleCGI(int ind);
	static void addCGI(int ind);
	static void delCGI(CGI_Info *ref);
};

#endif