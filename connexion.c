#include <unistd.h>

#include "connexion.h"

co_t** cos;
int nbCos;

int co_get_addr(const co_t* co, char * dst,const socklen_t size)
{
    if(!co)
    {
        perror("Error : get_co_addr() : co = null\n");
        return -1;
    }
    if(inet_ntop (co->addr->sin6_family, &co->addr->sin6_addr, dst, size)==NULL)
    {
        perror("Error : get_co_addr() : inet_ntop\n");
        return -1;
    }
    return 0;
}
uint16_t co_get_port(const co_t* co)
{
    return co->addr->sin6_port;
}
int co_handle_new_pkt(const co_t* co, const pkt_t* pkt)
{
    co->lastSeqnum = pkt_get_seqnum(pkt);
    if(co->lastSeqnum != co->reqSeqnum)
    {
        int i=0;
        for(int i=0;i<MAX_WINDOW_SIZE;i++)
            if(co->buffer[i] == NULL)
                break;
        co->buffer[i] = malloc(sizeof pkt);
        if(!co->buffer[i])
            return -1;
        memcpy(co->buffer[i], pkt, sizeof pkt);
    }
    else
    {
        char *filename[strlen(FORMAT)+NB_MAX_CHAR_UINT32];
        int f = 1;
        if(FORMAT != NULL)
        {
            int i=0;
            do{
                sprintf(filename, FORMAT, i);
                i++;
            }while(access(filename, F_OK ) != -1);
            f = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
            if(f==-1){
                perror("co_handle_new_pkt() : Failed to open file");
                return -1;
            }
        }

        if(write(f,..))// to - do write pkt and all buffer
            return -1;

        //then erase buffer
        if(f>1 && close(f)==-1)
            return -1;

    }
}

co_t** malloc_connexions(const int nbConnexions)
{
    cos = malloc(nbConnexions * sizeof(co_t*));
    if(cos)
        nbCos = nbConnexions;
    else
        nbCos = 0;
    return cos;
}
int get_index_client(const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    int i=0;
    for(;i<nbCos;i++){
        if(cos[i]==NULL)
            return -1;
        if(cos[i]->addrSize == addrSize)
            if(memcmp(cos[i]->addr, addr, (size_t)addrSize) == 0)
                return i;
    }
    return -1;
}
int add_connexion(const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    int i=0;
    for(;i<nbCos;i++)
        if(cos[i]==NULL)
            break;
    cos[i] = malloc(sizeof co_t);
    if(!cos[i])
        return -1;
    cos[i]->addr = malloc(addrSize);
    if(!cos[i]->addr)
        return -1;
    cos[i]->addrSize = addSize;
    memcpy(cos[i]->addr, addr, addrSize);
    return 0;
}
void handle_reception(SOCKET sock, co_t** cos, int nbCos)
{
    SOCKADDR_IN6 *from = NULL;
    socklen_t fromSize;
    pkt_t *pRec = recv_pkt(sock, &from, &fromSize);

    int indClient = get_index_client(from, fromSize);
    if(indClient == -1)
        add_connexion(from, fromSize);



    print_sockaddr_in6(from);
    printPkt(pRec);
    uint8_t seqnum = pkt_get_seqnum(pRec);
    if(true){//pkt_get_window(pRec)>0){
        pkt_t *pSend = pkt_new();
        if(pkt_get_tr(pRec)){
            pkt_set_type(pSend, PTYPE_NACK);
            pkt_set_seqnum(pSend, seqnum);
        }
        else{
            pkt_set_type(pSend, PTYPE_ACK);
            pkt_set_seqnum(pSend, seqnum+1);
        }
        pkt_set_window(pSend,1);
        pkt_set_timestamp(pSend, pkt_get_timestamp(pRec));
        size_t n = send_pkt(sock, pSend, from, fromSize);
        printf("%ld bytes sent !\n",n);
    }
    free(from);
    pkt_del(pRec);
}
