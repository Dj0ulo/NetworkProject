#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "packet.h"
#include "socket.h"

#define TIMEOUT 2000
#define ADDR_MAX_LEN INET6_ADDRSTRLEN

#define DONE 1

typedef struct Connection co_t;
struct Connection
{
    SOCKADDR_IN6 *addr;
    socklen_t addrSize;
    char *filename;

    pkt_t* win[MAX_WINDOW_SIZE];
    uint8_t reqSeqnum;
    uint8_t winOffset;

    pkt_t* lastPktRecv, *lastPktSend;
    time_t timeLastPkt;

    bool gotNULL;
};

int co_reset(co_t* co, const SOCKADDR_IN6 *addr,const socklen_t addrSize);
void co_free(co_t *co);
int co_get_addr(const co_t* co, char * dst,const socklen_t size);
uint16_t co_get_port(const co_t* co);
bool co_is_timeout(const co_t* co);
int co_win_nb_hole(const co_t* co);
int co_handle_new_pkt(co_t* co, const pkt_t* pkt);
int co_send_req(co_t* co);



#endif // CONNECTION_H_INCLUDED
