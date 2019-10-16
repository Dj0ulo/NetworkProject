#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <fcntl.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */

#include "packet_interface.h"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;
typedef struct sockaddr_in6 SOCKADDR_IN6;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct addrinfo ADDRINFO;
typedef struct in6_addr IN6_ADDR;
typedef struct timeval TIMEVAL;

void set_socket_non_blocking(SOCKET sock);
SOCKADDR_IN6 bind_socket(SOCKET *sock, const char * hostname, const char * port);
pkt_t* recv_pkt(SOCKET sock, SOCKADDR_IN6** from, socklen_t *fromSize);
size_t send_pkt(SOCKET sock, const pkt_t *pkt,const SOCKADDR_IN6* to, const socklen_t toSize);


#endif // SOCKET_H_INCLUDED
