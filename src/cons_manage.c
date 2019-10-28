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
    if(pRec!=NULL)
    {
        int indClient = get_index_client(from, fromSize);
//        printf("Connection : "YELLOW"%d - "WHITE,indClient);
//        print_sockaddr_in6(from);

//    if(rand()%4==0){
//        fprintf(stderr,YELLOW "FAKE NOT RECV\n" WHITE);
//        free(from);
//        pkt_del(pRec);
//        return 0;
//    }

        //pkt_print(pRec);

        if(indClient == -1){
            indClient = add_connection(from, fromSize);
            printf("New connection id : %d\n", indClient);
        }
        if(indClient != -1)
        {
            int r = co_handle_new_pkt(cons[indClient],pRec);
            if(r == -1)
                ;//err "handle_reception() : Unable to handle new packet" ne
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
    else
    {
        //err "handle_reception() : Packet unconsistent received\n" ne
        ret = -1;
    }

    free(from);
    pkt_del(pRec);
    free(pRec);
    return ret;
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
