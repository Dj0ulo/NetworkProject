#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "packet.h"

extern SOCKET sock;
extern char *FORMAT;

int co_reset(co_t* co, const SOCKADDR_IN6 *addr,const socklen_t addrSize)
{
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

    co->timeStart = millis();
    co->bytesWrote = 0;
    co->file = -1;
    co->offBigBuf = 0;

    co->winOffset = 0;
    co->gotNULL = false;
    return 0;
}
void co_free(co_t *co)
{
    if(co)
    {
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
    prt("Window : |");
    for(int i=0;i<MAX_WINDOW_SIZE;i++)
        prt("%3d|",i);
    prt("\n         |");
    uint8_t sn = (MAX_WINDOW_SIZE - co->winOffset) +  co->reqSeqnum;
    for(int i=0;i<MAX_WINDOW_SIZE;i++)
    {
        if(i==co->winOffset){
            sn = co->reqSeqnum;
            prt(CYAN);
        }
        else if(co->win[i]){
            prt(GREEN);
        }
        else{
            prt(WHITE);
        }
        prt("%3d"WHITE"|", sn);
        sn++;
    }
    prt("\nReq seqnum : %d\n", co->reqSeqnum);
}
int co_handle_new_pkt(co_t* co, const pkt_t* pkt)
{
    if(pkt == NULL){
        prt(RED  "co_handle_new_pkt() : Packet unconsistent received\n" ne
        if(co_send_req(co)==-1)
            prt(RED  "co_handle_new_pkt() : Unable to send packet" ne
        return -1;
    }
    pkt_print(pkt);
    pkt_del(co->lastPktRecv);

    if(pkt_copy(co->lastPktRecv, pkt)==-1){err "co_handle_new_pkt() : Unable to copy the packet" ner}
    if(pkt_get_tr(pkt))
    {
        prt(RED "co_handle_new_pkt() : truncated packet" ne
        if(co_send_req(co)==-1)
            err "handle_reception() : Unable to send packet" ne
        return -1;
    }
    const uint8_t sn = pkt_get_seqnum(pkt);
    bool goodSn = co->reqSeqnum <= sn && sn < (int)co->reqSeqnum + MAX_WINDOW_SIZE;
    goodSn = goodSn || (co->reqSeqnum <= (int)sn+0x100 && (int)sn+0x100 < (int)co->reqSeqnum + MAX_WINDOW_SIZE);
    if(!goodSn)
    {
        prt(RED "co_handle_new_pkt() : beyond window size" ne
        if(co_send_req(co)==-1)
            err "handle_reception() : Unable to send packet" ne
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

    //ouverture fichier
    if(co->file == -1)
    {
        if(!FORMAT)
        {
            char format[] = "file_recv_%d.dat";
            FORMAT = malloc(strlen(format)+1);
            if(!FORMAT) {
                err "co_handle_new_pkt() : allocating FORMAT" ne
            }
            else{
                strcpy(FORMAT, format);
            }
        }
        char filename[strlen(FORMAT)+NB_MAX_CHAR_UINT32];
        int i=0;
        do{
            sprintf(filename, FORMAT, i);
            i++;
        }while(access(filename, F_OK ) != -1);
        co->file = open(filename, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU);
        if(co->file==-1){err "co_handle_new_pkt() : Failed to open file\n" ne}
    }

    //ecriture
    int i=0;
    for(;i<MAX_WINDOW_SIZE;i++)
    {
        uint8_t ind = (i+co->winOffset)%MAX_WINDOW_SIZE;
        pkt_t *buf = co->win[ind];
        if(!buf)
            break;

        //speed
        const ssize_t len = pkt_get_length(buf);//
        co->bytesWrote += len;
        float speed = co->bytesWrote*1000.0/((float)millis()-co->timeStart);
        prt(RED"%.2f Ko "WHITE"Speed %.2f KB/s\n"WHITE,(float)co->bytesWrote/1024.0,speed/1000.0);

        //writing
        if(co->offBigBuf + len >= SIZE_BIG_BUF){
            co_write_big_buf(co);
        }
        memcpy(co->bigBuf + co->offBigBuf, pkt_get_payload(buf), len);
        co->offBigBuf += len;

        pkt_del(co->win[ind]);
        free(co->win[ind]);
        co->win[ind] = NULL;
        co->reqSeqnum++;
    }
    co->winOffset = (co->winOffset+i)%MAX_WINDOW_SIZE;
    print_window(co);

    if(pkt_get_length(pkt)==0)
        co->gotNULL = true;
    if(co_send_req(co)==-1)
        err "handle_reception() : Unable to send packet" ne

    if((co->gotNULL && co_win_nb_hole(co) == MAX_WINDOW_SIZE) || millis() - co->timeLastPkt > TIMEOUT*5)
    {
        if(millis() - co->timeLastPkt > TIMEOUT*5){
            fprintf(stderr,RED "Connection lost\n");
        }
        co_write_big_buf(co);//write last bytes
        close(co->file);

        fprintf(stderr,GREEN "Done "WHITE);
        print_sockaddr_in6(co->addr);
        fprintf(stderr,"%.2f KB in %.3f sec - av. speed %.2f Ko/s\n",co->bytesWrote/1024.0f,(millis()-co->timeStart)/1000.0f,
                co->bytesWrote/1.024f/(millis()-co->timeStart));
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

    //pkt_set_window(pSend, co_win_nb_hole(co));
    pkt_set_window(pSend, MAX_WINDOW_SIZE);
    prt("Win hole : %d\n", pkt_get_window(pSend));
    pkt_set_timestamp(pSend, pkt_get_timestamp(co->lastPktRecv));

    size_t n = send_pkt(sock, pSend, co->addr, co->addrSize);
    co->timeLastPkt = millis();
    prt(YELLOW"SEND : \n%ld"WHITE" bytes sent ! (%dACK: "CYAN"%d"WHITE")\n",n,pkt_get_tr(co->lastPktRecv), co->reqSeqnum);


    pkt_del(co->lastPktSend);

    pkt_copy(co->lastPktSend, pSend);
    pkt_del(pSend);
    free(pSend);
    return 0;
}
void co_write_big_buf(co_t* co){
    prt("Write big buff %ld bytes\n", co->offBigBuf);
    write_bytes(co->file, co->bigBuf, co->offBigBuf);
    co->offBigBuf = 0;
}
