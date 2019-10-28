#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "packet.h"

extern SOCKET sock;
extern char *FORMAT;

int co_reset(co_t* co, const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
    co->filename = calloc(1,1);
    if(!co->filename)
        return -1;
    co->addr = malloc(addrSize);
    if(!co->addr)
        return -1;
    co->addrSize = addrSize;
    memcpy(co->addr, addr, addrSize);

    for(int i=0;i<MAX_WINDOW_SIZE;i++)
        co->win[i] = NULL;

    pkt_del(co->lastPktRecv);
    co->lastPktRecv = pkt_new();

    pkt_del(co->lastPktSend);
    co->lastPktSend = pkt_new();
    co->reqSeqnum = 0;

    co->winOffset = 0;
    co->gotNULL = false;
    return 0;
}
void co_free(co_t *co)
{
    if(co)
    {
        if(co->filename)
            free(co->filename);
        if(co->addr)
            free(co->addr);

        for(int k=0;k<MAX_WINDOW_SIZE;k++){
            pkt_del(co->win[k]);
            if(co->win[k])
                free(co->win[k]);
        }
        pkt_del(co->lastPktRecv);
        if(co->lastPktRecv)
            free(co->lastPktRecv);
        pkt_del(co->lastPktSend);
        if(co->lastPktSend)
            free(co->lastPktSend);
    }
}
int co_get_addr(const co_t* co, char * dst,const socklen_t size)
{
    if(!co)
    {
        fprintf(stderr,"Error : get_co_addr() : co = null\n");
        return -1;
    }
    if(inet_ntop (co->addr->sin6_family, &co->addr->sin6_addr, dst, size)==NULL)
    {
        fprintf(stderr,"Error : get_co_addr() : inet_ntop\n");
        return -1;
    }
    return 0;
}
uint16_t co_get_port(const co_t* co)
{
    return co->addr->sin6_port;
}

bool co_is_timeout(const co_t* co)
{
    return millis() - co->timeLastPkt > TIMEOUT;
}
int co_win_nb_hole(const co_t* co)
{
    int n=0;
    int i=co->winOffset;
    for(;i<MAX_WINDOW_SIZE+co->winOffset;i++)
    {
        if(co->win[i%MAX_WINDOW_SIZE])
            break;
        else
            n++;
    }
    return n;
}
void print_window(const co_t* co)
{
    printf("Window : |");
    for(int i=0;i<MAX_WINDOW_SIZE;i++)
        printf("%3d|",i);
    printf("\n         |");
    uint8_t sn = (MAX_WINDOW_SIZE - co->winOffset) +  co->reqSeqnum;
    for(int i=0;i<MAX_WINDOW_SIZE;i++)
    {
        if(i==co->winOffset){
            sn = co->reqSeqnum;
            printf(CYAN);
        }
        else if(co->win[i])
            printf(GREEN);
        else
            printf(WHITE);
        printf("%3d"WHITE"|", sn);
        sn++;
    }
    printf("\nReq seqnum : %d\n", co->reqSeqnum);
}
int co_handle_new_pkt(co_t* co, const pkt_t* pkt)
{
    pkt_del(co->lastPktRecv);

    if(pkt_copy(co->lastPktRecv, pkt)==-1){err "co_handle_new_pkt() : Unable to copy the packet" ner}
    if(pkt_get_tr(pkt))
    {
        err "co_handle_new_pkt() : truncated packet" ner
    }
    const uint8_t sn = pkt_get_seqnum(pkt);
    bool goodSn = co->reqSeqnum <= sn && sn < (int)co->reqSeqnum + MAX_WINDOW_SIZE;
    goodSn = goodSn || (co->reqSeqnum <= (int)sn+0x100 && (int)sn+0x100 < (int)co->reqSeqnum + MAX_WINDOW_SIZE);
    if(!goodSn)
    {
        //err "co_handle_new_pkt() : beyond window size" ner
        return -1;
    }

    int indRcv = co->winOffset;
    if(sn < co->reqSeqnum)
        indRcv += 0x100 - co->reqSeqnum + sn;
    else
        indRcv += sn - co->reqSeqnum;

    co->win[indRcv%MAX_WINDOW_SIZE] = pkt_new();
    if(pkt_copy(co->win[indRcv%MAX_WINDOW_SIZE], pkt)==-1)
        return -1;

    bool newCo = false;
    //ouverture fichier
    if(strcmp(co->filename,"") == 0)
    {
        newCo = true;
        if(!FORMAT)
        {
            char format[] = "file_recv_%d.dat";
            FORMAT = malloc(strlen(format)+1);
            if(!FORMAT) {
                err "co_handle_new_pkt() : allocating FORMAT" ner
            }
            strcpy(FORMAT, format);
        }

        co->filename = realloc(co->filename, strlen(FORMAT)+NB_MAX_CHAR_UINT32);
        if(!co->filename){
            err "co_handle_new_pkt() : malloc" ner
        }

        int i=0;
        do{
            sprintf(co->filename, FORMAT, i);
            i++;
        }while(access(co->filename, F_OK ) != -1);
    }
    int f = open(co->filename, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
    if(f==-1){err "co_handle_new_pkt() : Failed to open file\n" ner}


    //ecriture
    int i=0;
    for(;i<MAX_WINDOW_SIZE;i++)
    {
        uint8_t ind = (i+co->winOffset)%MAX_WINDOW_SIZE;
        pkt_t *buf = co->win[ind];
        if(!buf)
            break;
        //printf(GREEN"Writing seqnum "CYAN"%d"GREEN" (win ind : %d) in %s\n"WHITE, pkt_get_seqnum(buf),ind, co->filename);
        if(write_bytes(f, pkt_get_payload(buf), pkt_get_length(buf))==-1){
            err "co_handle_new_pkt() : write" ner
        }
        pkt_del(co->win[ind]);
        free(co->win[ind]);
        co->win[ind] = NULL;
        co->reqSeqnum++;
    }
    co->winOffset = (co->winOffset+i)%MAX_WINDOW_SIZE;
    if(close(f)==-1)
        return -1;
    //print_window(co);

    if(pkt_get_length(pkt)==0)
        co->gotNULL = true;
    bool isLastOfWindow = pkt_get_seqnum(co->lastPktSend) + pkt_get_window(co->lastPktSend) - 1 == pkt_get_seqnum(pkt);
    if(isLastOfWindow || co->gotNULL || newCo)
        if(co_send_req(co)==-1)
            err "handle_reception() : Unable to send packet" ne
    //printf(MAGENTA"_______________________\n\n"WHITE);
    if(co->gotNULL && co_win_nb_hole(co) == MAX_WINDOW_SIZE)
    {
        fprintf(stderr, GREEN"Done with : "WHITE);
        print_sockaddr_in6(co->addr);
        return DONE;
    }

    return 0;
}
int co_send_req(co_t* co)
{
    if(!co){err"co_send_req() : Bad connection\n"ner}
    if(!co->addr){err"co_send_req() : Bad address"ner}
    if(!co->lastPktRecv){err"co_send_req() : No last pkt"ner}

    pkt_t *pSend = pkt_new();
    if(pkt_get_tr(co->lastPktRecv))
        pkt_set_type(pSend, PTYPE_NACK);
    else
        pkt_set_type(pSend, PTYPE_ACK);

    pkt_set_seqnum(pSend, co->reqSeqnum);

    pkt_set_window(pSend, co_win_nb_hole(co));
    //printf("Win hole : %d\n", pkt_get_window(pSend));
    pkt_set_timestamp(pSend, pkt_get_timestamp(co->lastPktRecv));

    /*size_t n = */send_pkt(sock, pSend, co->addr, co->addrSize);
    co->timeLastPkt = millis();
    //printf(YELLOW"%ld"WHITE" bytes sent ! (need seqnum : "CYAN"%d"WHITE")\n",n, co->reqSeqnum);

    pkt_del(co->lastPktSend);

    pkt_copy(co->lastPktSend, pSend);
    pkt_del(pSend);
    free(pSend);
    return 0;
}
