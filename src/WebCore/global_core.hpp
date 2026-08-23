// general\global functionalies used in WebCore but shared ammong full code

#ifndef JJ_GLOBAL_CORE_HPP
# define JJ_GLOBAL_CORE_HPP

// General libs tbr start
# include <iostream>
# include <algorithm>
# include <vector>
# include <map>
# include <string>

# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <poll.h>
# include <sys/epoll.h>
// General libs tbr end

# include "structs.hpp"
# include "macros.hpp"

// classes
# include "Info.hpp"
# include "Sockets.hpp"
# include "ServerInfo.hpp"
# include "ClientInfo.hpp"
# include "Excpetions.hpp"

#endif