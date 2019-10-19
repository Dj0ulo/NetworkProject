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
#include <time.h>

#include "packet.h"
#include "socket.h"
#include "cons_manage.h"
#include "utils.h"

SOCKET sock = 0;
char *FORMAT = NULL;

int main(int argc, char *argv[])
{
    printf("Hello world!\n");
    srand(time(NULL));
    startClock();

    char *HOSTNAME = NULL, *PORT = NULL;
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
        fprintf(stderr,"HOSTNAME malloc()"); exit(errno);
    }
    strcpy(HOSTNAME,argv[optind]);

    PORT = malloc(strlen(argv[optind+1])+1);
    if(!PORT){
        fprintf(stderr,"PORT malloc()"); exit(errno);
    }
    strcpy(PORT,argv[optind+1]);

    //printf("%s : %s - N: %d\n",HOSTNAME, PORT, N_CONNEXIONS);
    malloc_connections(N_CONNEXIONS);


    SOCKADDR_IN6 sin6 = bind_socket(&sock, HOSTNAME, PORT);
    char addrstr[100];
    inet_ntop (sin6.sin6_family, &sin6.sin6_addr, addrstr, 100);
    printf("%s : %d\n",addrstr, ntohs(sin6.sin6_port) );

    if(sock == INVALID_SOCKET) {
        fprintf(stderr,"socket()"); exit(errno);
    }

    bool ra = true;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ra, sizeof(ra));

    fd_set fdSet;
    TIMEVAL timeout;
    while(true)
    {
        FD_ZERO(&fdSet);
        FD_SET(sock, &fdSet);

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        int ready = select(sock + 1, &fdSet, NULL, NULL, &timeout);
        if (ready < 0) {
			fprintf(stderr,"select()");   exit(EXIT_FAILURE);
		}
		else if(ready == 0)
        {
            //fprintf(stderr,CYAN"Time : %ld s.\n"WHITE, millis()/1000);//fprintf(stderr,".");
        }
		else if(FD_ISSET(sock,&fdSet)){
            printf("Got something !\n");
            handle_reception();
		}

		check_times_out();
    }

//    char buffer[MAX_PACKET_SIZE];
//    struct hostent *hostinfo = NULL;
//    SOCKADDR_IN from = { 0 };
//    int fromsize = sizeof from;
//
//    if((n = recvfrom(sock, buffer, MAX_PACKET_SIZE, 0, (SOCKADDR *)&from, &fromsize)) < 0)
//    {
//        fprintf(stderr,"recvfrom()");
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
    free_connections();
    close(sock);
    free(HOSTNAME);
    free(PORT);
    if(FORMAT)
        free(FORMAT);

    return 0;
}
