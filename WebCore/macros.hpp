#ifndef JJMACROS_HPP
# define JJMACROS_HPP

// protocol for the connection, standard is ipv4
# ifndef DOMAIN
#  define DOMAIN AF_INET
# endif

// type of connection, standard is stream of chars
# ifndef TYPE
#  define TYPE SOCK_STREAM
# endif

// additional flags to use in the connection, standard is no flags
# ifndef PROTOCOL
#  define PROTOCOL 0
# endif

// what kind of address's to accept a connection from, standard is all
# ifndef INTERFACE
#  define INTERFACE INADDR_ANY
# endif

// what port are we listening from, standard is 8080
# ifndef PORT
#  define PORT 8080
# endif

// what port are we listening from, standard is 8080
# ifndef QUEUE
#  define QUEUE 10
# endif

// max size for HTTP requests, standard is 10000
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1000
# endif

// Wait time to accept connections
# ifndef WAIT
#  define WAIT 500
# endif

#endif