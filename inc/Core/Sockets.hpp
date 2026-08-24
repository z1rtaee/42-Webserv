#ifndef JJ_SOCKETS_HPP
# define JJ_SOCKETS_HPP

#include "Webserv.hpp"

class Sockets
{
public:
	static std::vector<struct pollfd>	AllSockets; // Vector with all Sockets to be monitored by the poll() loop.
	static std::vector<Info *>			SocketInfo; // Vector with all info related the sockets

	int	operator[](int ind);
	Sockets(); // adds Socket and related information to the list of know Sockets
	~Sockets();

	// static void	delEverything(int SocketID); // deletes the SocketID from the list of sockets, does nothing if it doesnt exist

	static int	find(int SocketID); // returns the index of the SocketID on success, returns -1 on error

	static void	addServer(t_info &Config); // adds the ServerSocketID from the list of ServerSockets, does nothing if it doesnt exist

	static void	addClient(int ind); // adds the ClientSocketID from the list of ClientSockets, does nothing if it doesnt exist

	static void	delEverything(); // deletes the SocketID from the list of sockets, does nothing if it doesnt exist

	static void ClientRequest(int ind);

	static void ServerResponse(int ind);

	static void	delSocket(int ind); // deletes the SocketID from the list of sockets, does nothing if it doesnt exist
};

#endif