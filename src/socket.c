#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "socket.h"
#include "utils.h"

void print_sockaddr_in6(const SOCKADDR_IN6 *sin6)
{
    printf(CYAN"sockaddr_in6" WHITE " : ");
    if(!sin6)
        printf(RED"(NULL)\n"WHITE);
    else{
        char addrstr[INET6_ADDRSTRLEN];
        inet_ntop (sin6->sin6_family, &sin6->sin6_addr, addrstr, INET6_ADDRSTRLEN);
        printf (RED "%s" WHITE,addrstr);
        printf(" : " MAGENTA "%d" WHITE" \n", sin6->sin6_port);
    }
}

void set_socket_non_blocking(SOCKET sock){
    int opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	if (fcntl(sock, F_SETFL, opts | O_NONBLOCK) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
}

SOCKADDR_IN6 bind_socket(SOCKET *sock, const char * hostname, const char * port)
{
    ADDRINFO hints, *res = NULL;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */


    int s = getaddrinfo(hostname, port , &hints, &res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    SOCKADDR_IN6 sin6 = { 0 };
    while (res)
    {
        sin6 = *((SOCKADDR_IN6 *) res->ai_addr);

        SOCKET sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR){
            perror("bind()\n");   exit(errno);
        }
        else
        {
            //perror("Bind success\n");
            *sock = sfd;
            break;//success
        }
        close(sfd);
        res = res->ai_next;
    }

    if (res == NULL) {               /* No address succeeded */
        perror("Could not bind\n");
        exit(EXIT_FAILURE);
    }
    if(res)
        freeaddrinfo(res);           /* No longer needed */
    return sin6;
}
pkt_t* recv_pkt(SOCKET sock, SOCKADDR_IN6** from, socklen_t *fromSize)
{
    ssize_t n = 0;

    *fromSize = sizeof(SOCKADDR_STORAGE);
    SOCKADDR_STORAGE recv_addr;

    char buffer[MAX_PACKET_SIZE];
    if((n = recvfrom(sock, buffer, MAX_PACKET_SIZE, 0, (SOCKADDR *)&recv_addr, fromSize)) < 0){
        perror("recvfrom()"); exit(errno);
    }
    *from = malloc(*fromSize);
    if(!*from){
        perror("from malloc()"); exit(errno);
    }
    memcpy(*from, &recv_addr, *fromSize);

    pkt_t *pRec = pkt_new();
    pkt_status_code r = 0;
    if((r=pkt_decode(buffer, n, pRec))!=PKT_OK){
        fprintf(stderr,"pkt_decode() : %d",r);
        pkt_del(pRec);
        free(pRec);
        return NULL;
    }
    return pRec;
}
size_t send_pkt(SOCKET sock, const pkt_t *pkt,const SOCKADDR_IN6* to, const socklen_t toSize)
{
    ssize_t n = 0;

    char buffer[MAX_PACKET_SIZE];
    size_t len = MAX_PACKET_SIZE;
    pkt_status_code r = 0;
    if((r=pkt_encode(pkt, buffer, &len))!=PKT_OK){
        fprintf(stderr,"pkt_encode() : %d",r); return -1;
    }
    if((n = sendto(sock, buffer, len, 0, (SOCKADDR *)to, toSize)) < 0) {
        perror("sendto()"); exit(errno);
    }
    return (size_t)n;
}
