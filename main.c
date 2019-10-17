/*
 *  Sources :
 *  https://broux.developpez.com/articles/c/sockets/#L3-4-2
 *  https://gist.github.com/jirihnidek/bf7a2363e480491da72301b228b35d5d
 *  https://beej.us/guide/bgnet/html/multi/sockaddr_inman.html
 *  https://www.lowtek.com/sockets/select.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include "packet_interface.h"
#include "socket.h"
#include "utils.h"


char *FORMAT = NULL;

int main(int argc, char *argv[])
{
    printf("Hello world!\n");

    char, *HOSTNAME = NULL, *PORT = NULL;
    //uint16_t port = 0;
    int N_CONNEXIONS = 100;
    char c;
    while (argc > 1 && (c = getopt (argc, argv, "o:m:")) != -1)
    {
        switch (c)
        {
        case 'o':
			N_CONNEXIONS = atoi(optarg);
            break;
        case 'm':
            FORMAT = malloc(strlen(optarg)+1);
            if(!FORMAT) {
                fprintf(stderr,"ERROR allocating FORMAT\n"); return -1;
            }
            strcpy(FORMAT, optarg);
            break;
        case '?':
			fprintf(stderr,"ERROR argument\n");
            return -1;
        }
    }
	if(optind +1 >= argc) {
	    fprintf(stderr,"ERROR need Hostname and port\n"); return -1;
	}

    HOSTNAME = malloc(strlen(argv[optind])+1);
    if(!HOSTNAME){
        perror("HOSTNAME malloc()"); exit(errno);
    }
    strcpy(HOSTNAME,argv[optind]);

    PORT = malloc(strlen(argv[optind+1])+1);
    if(!PORT){
        perror("PORT malloc()"); exit(errno);
    }
    strcpy(PORT,argv[optind+1]);

    //port = atoi(argv[optind+1]);

    printf("%s : %s - N: %d\n",HOSTNAME, PORT, N_CONNEXIONS);


    //SOCKET clients[N_CONNEXIONS];
    SOCKET sock = 0;
    SOCKADDR_IN6 sin6 = bind_socket(&sock, HOSTNAME, PORT);
    char addrstr[100];
    inet_ntop (sin6.sin6_family, &sin6.sin6_addr, addrstr, 100);
    printf("%s : %d\n",addrstr, ntohs(sin6.sin6_port) );

    if(sock == INVALID_SOCKET) {
        perror("socket()"); exit(errno);
    }

    bool ra = true;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ra, sizeof(ra));

    fd_set fdSet;
    TIMEVAL timeout;
    while(true){
        FD_ZERO(&fdSet);
        FD_SET(sock, &fdSet);

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        int ready = select(sock + 1, &fdSet, NULL, NULL, &timeout);
        if (ready < 0) {
			perror("select()");   exit(EXIT_FAILURE);
		}
		else if (ready == 0) { //Nothing ready to read, just show that we're alive
			printf(".");
			fflush(stdout);
		}
		else if(FD_ISSET(sock,&fdSet)){
            printf("Got something !\n");
            //struct hostent *hostinfo = NULL;
            SOCKADDR_IN6 *from = NULL;
            socklen_t fromSize;
            pkt_t *pRec = recv_pkt(sock, &from, &fromSize);
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
//                    if(seqnum == 3)
//                        pkt_set_seqnum(pSend, seqnum);
//                    else
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
    }

//    char buffer[MAX_PACKET_SIZE];
//    struct hostent *hostinfo = NULL;
//    SOCKADDR_IN from = { 0 };
//    int fromsize = sizeof from;
//
//    if((n = recvfrom(sock, buffer, MAX_PACKET_SIZE, 0, (SOCKADDR *)&from, &fromsize)) < 0)
//    {
//        perror("recvfrom()");
//        exit(errno);
//    }

    //const uint16_t x = 0x8000-1;

//    printBits(0x5c);
//    printBits((0x5c & 0b11000000) >> 6);
//
//    pkt_t * pkt = pkt_new();
//    pkt_set_payload(pkt,"hello world",11);
//    pkt_set_window(pkt,28);
//    pkt_set_seqnum(pkt,0x7b);
//    pkt_set_timestamp(pkt, 0x17);
//
//
//    char buf[MAX_PACKET_SIZE];
//    size_t len = MAX_PACKET_SIZE;
//    printf("%d",pkt_encode(pkt, buf, &len));
//    printPkt(pkt);
//    printHex((uint8_t*)buf, len);
//
//    pkt_t *pRec = pkt_new();
//    printf("%d",pkt_decode(buf, len,pRec));
//    printPkt(pRec);

//    uint8_t *data = malloc(2);
//    printf("%u, size %ld\n",x, varuint_encode(x,data,2) );
//    printBits(*(uint16_t*)data);
//    uint16_t ret;
//    printf("size %ld\n",varuint_decode(data,2,&ret));
//    printBits(ret);
////    free(data);
//    pkt_del(pRec);
//    pkt_del(pkt);

    close(sock);
    free(HOSTNAME);
    free(PORT);
    free(FORMAT);
    return 0;
}
