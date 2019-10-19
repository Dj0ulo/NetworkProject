#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "cons_manage.h"

co_t** cons;
int NB_CONNECTIONS;
extern SOCKET sock;

co_t** malloc_connections(const int nbConnections)
{
    cons = malloc(nbConnections * sizeof(co_t*));
    if(cons)
        NB_CONNECTIONS = nbConnections;
    else
        NB_CONNECTIONS = 0;
    return cons;
}
void free_connections()
{
    for(int i=0;i<NB_CONNECTIONS;i++)
        co_free(cons[i]);
}
int get_index_client(const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    int i=0;
    for(;i<NB_CONNECTIONS;i++){
        if(cons[i]==NULL)
            return -1;
        if(cons[i]->addrSize == addrSize)
            if(memcmp(cons[i]->addr, addr, (size_t)addrSize) == 0)
                return i;
    }
    return -1;
}
int add_connection(const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    int i=0;
    for(;i<NB_CONNECTIONS;i++)
        if(cons[i]==NULL)
            break;
    if(i==NB_CONNECTIONS)
        return -1;
    cons[i] = malloc(sizeof(co_t));
    if(!cons[i])
        return -1;
    co_reset(cons[i], addr, addrSize);

    return i;
}
int handle_reception()
{
    SOCKADDR_IN6 *from = NULL;
    socklen_t fromSize;
    pkt_t *pRec = recv_pkt(sock, &from, &fromSize);
    if(pRec==NULL){err "handle_reception() : Packet unconsistent received\n" ner}

    /*if(rand()%4==0){
        fprintf(stderr,YELLOW "FAKE NOT RECV\n" WHITE);
        return 0;
    }*/
    //print
    print_sockaddr_in6(from);

    int indClient = get_index_client(from, fromSize);
    //printf("--> Index client : "YELLOW"%d\n"WHITE,indClient);

    pkt_print(pRec);

    if(indClient == -1){
        indClient = add_connection(from, fromSize);
    }
    if(indClient == -1){err "handle_reception() : Too many connexions\n" ner}
    int r = co_handle_new_pkt(cons[indClient],pRec);
    if(r == -1)
        err "handle_reception() : Unable to handle new packet" ne
    else if(r == DONE)
    {
        co_free(cons[indClient]);
        cons[indClient] = NULL;
    }

    free(from);
    pkt_del(pRec);
    return 0;
}
void check_times_out()
{
    for(int i=0;i<NB_CONNECTIONS;i++)
    {
        if(cons[i] && cons[i]->lastPktRecv && co_is_timeout(cons[i])){
            fprintf(stderr,RED"TIMEOUT : "WHITE);
            print_sockaddr_in6(cons[i]->addr);
            co_send_req(cons[i]);
        }
    }
}
