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

#include "packet.h"

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

/*
 *  Print une adresse pour debug
 */
void print_sockaddr_in6(const SOCKADDR_IN6 *sin6);

/*
 *  Met un socket en mode non bloquanr
 */
void set_socket_non_blocking(SOCKET sock);

/*
 *  Bind le socket avec un nom d'hote et un port
 *  @return l'adresse correspondante
 */
SOCKADDR_IN6 bind_socket(SOCKET *sock, const char * hostname, const char * port);

/*
 *  Recoit le packet en attente sur le socket et écrit l'adresse du client dans from
 *  @return le packet
 */
pkt_t* recv_pkt(SOCKET sock, SOCKADDR_IN6** from, socklen_t *fromSize);

/*
 *  Envoie le packet pkt sur le socket à l'adresse du client to
 *  @return le nombre d'octet envoyé
 */
size_t send_pkt(SOCKET sock, const pkt_t *pkt,const SOCKADDR_IN6* to, const socklen_t toSize);


#endif // SOCKET_H_INCLUDED
