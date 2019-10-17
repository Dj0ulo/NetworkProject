#ifndef CONNEXION_H_INCLUDED
#define CONNEXION_H_INCLUDED

#include "packet_interface.h"
#include "socket.h"

#define ADDR_MAX_LEN INET6_ADDRSTRLEN

typedef struct Connexion co_t;
struct Connexion
{
    SOCKADDR_IN6 *addr = NULL;
    socklen_t addrSize;
    pkt_t *buffer[MAX_WINDOW_SIZE] = {NULL};
    uint8_t hiSeqnum = 0, lastSeqnum = 0, reqSeqnum = 0;
};

int get_co_addr(const co_t*, char * dst,const socklen_t size);
uint16_t get_co_port(const co_t*);

bool isConnected()
void handle_reception(SOCKET sock, co_t**);


#endif // CONNEXION_H_INCLUDED
