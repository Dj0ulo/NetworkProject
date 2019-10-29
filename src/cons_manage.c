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
    cons = calloc(nbConnections, sizeof(co_t*));
    if(cons)
        NB_CONNECTIONS = nbConnections;
    else
        NB_CONNECTIONS = 0;
    return cons;
}
void free_connections()
{
    if(cons)
    {
        for(int i=0;i<NB_CONNECTIONS;i++)
            if(cons[i] != NULL){
                co_free(cons[i]);
                free(cons[i]);
            }
        free(cons);
    }

}
int get_index_client(const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    int i=0;
    for(;i<NB_CONNECTIONS;i++){
        if(cons[i] && cons[i]->addrSize == addrSize)
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
    int ret = 0;
    SOCKADDR_IN6 *from = NULL;
    socklen_t fromSize;
    pkt_t *pRec = recv_pkt(sock, &from, &fromSize);

    int indClient = get_index_client(from, fromSize);
    prt("Connection : "YELLOW"%d - "WHITE,indClient);
    print_sockaddr_in6(from);


    if(indClient == -1 && pRec!=NULL && pkt_get_seqnum(pRec)!=0)
    {
        err "Not for me (New connection and seqnum != 0)" ne
        ret = -1;
    }
    else
    {
        if(indClient == -1){
            indClient = add_connection(from, fromSize);
            fprintf(stderr,"New connection id : %d\n", indClient);
        }
        if(indClient != -1)
        {
            int r = co_handle_new_pkt(cons[indClient],pRec);//handle new packet
            if(r == -1){
                prt(RED"handle_reception() : Unable to handle new packet" ne
            }
            else if(r == DONE)
            {
                co_free(cons[indClient]);
                free(cons[indClient]);
                cons[indClient] = NULL;
            }
        }
        else
        {
            err "handle_reception() : Too many connexions\n" ne
            ret = -1;
        }
    }


    free(from);
    pkt_del(pRec);
    free(pRec);
    prt(MAGENTA"____________RCV-END___________\n\n"WHITE);
    return ret;
}
void check_times_out()
{
    for(int i=0;i<NB_CONNECTIONS;i++)
    {
        if(cons[i] && cons[i]->lastPktRecv && co_is_timeout(cons[i])){
            prt(RED"TIMEOUT : "WHITE);
            print_sockaddr_in6(cons[i]->addr);
            co_send_req(cons[i]);
        }
    }
}
