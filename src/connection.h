#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include "packet.h"
#include "socket.h"

#define TIMEOUT 2000
#define ADDR_MAX_LEN INET6_ADDRSTRLEN
#define SIZE_BIG_BUF 8*1024*1024

#define DONE 1

//Structure gérant une connection à un client
typedef struct Connection co_t;
struct Connection
{
    SOCKADDR_IN6 *addr;
    socklen_t addrSize;
    int file;
    char bigBuf[SIZE_BIG_BUF];
    size_t offBigBuf;

    pkt_t* win[MAX_WINDOW_SIZE];
    uint8_t reqSeqnum;
    uint8_t winOffset;

    pkt_t* lastPktRecv, *lastPktSend;
    time_t timeLastPkt, timeStart;
    size_t bytesWrote;

    bool gotNULL;
};

/*
 *  Initialise la Connection co avec l'adresse addr
 *  @return 0 si tout est ok, -1 sinon
 */
int co_reset(co_t* co, const SOCKADDR_IN6 *addr,const socklen_t addrSize);

/*
 *  Free la Connection co
 */
void co_free(co_t *co);

/*
 *  Ecrit l'adresse de co dans dst
 *  @return 0 si tout est ok, -1 sinon
 */
int co_get_addr(const co_t* co, char * dst,const socklen_t size);

/*
 *  @return le port de cot
 */
uint16_t co_get_port(const co_t* co);

/*
 *  Check si ca fait longtemps qu'on a pas recu de packet de co
 *  @return 1 si oui, 0 sinon
 */
bool co_is_timeout(const co_t* co);

/*
 *  @return le nombre de trou dans la fenetre depuis le plus petit seqnum demandé jusqu'au plus petit seqnum bien recu
 */
int co_win_nb_hole(const co_t* co);

/*
 *  Gère l'arrivée d'un nouveau packet venant de la Connection co
 *  (met le packet dans la window, ou l'écrit dans le fichier, et envoie le packet de retour)
 *  @return 0 si tout est ok, -1 sinon
 */
int co_handle_new_pkt(co_t* co, const pkt_t* pkt);

/*
 *  Envoie le packet de retour à l'addresse de co
 *  @return 0 si tout est ok, -1 sinon
 */
int co_send_req(co_t* co);



#endif // CONNECTION_H_INCLUDED
