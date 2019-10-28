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
#include "packet.h"
#include "socket.h"
#include "cons_manage.h"
#include "utils.h"

SOCKET sock = 0;
char *FORMAT = NULL;
bool doPrint = false;

int main(int argc, char *argv[])
{
    per " - RECEIVER -\n");

    startClock();
    int exitSec = -1;
    char *HOSTNAME = NULL, *PORT = NULL;
    int N_CONNEXIONS = 100;
    char c;
    while (argc > 1 && (c = getopt (argc, argv, "o:m:t:p")) != (char)-1)
    {
        switch (c)
        {
        case 'p':
			doPrint = true;
            break;
        case 't':
			exitSec = atoi(optarg);
            break;
        case 'm':
			N_CONNEXIONS = atoi(optarg);
            break;
        case 'o':
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
	    err "need Hostname and port\n" ner
	}

    HOSTNAME = malloc(strlen(argv[optind])+1);
    if(!HOSTNAME){
        err "HOSTNAME malloc()" ner
    }
    strcpy(HOSTNAME,argv[optind]);

    PORT = malloc(strlen(argv[optind+1])+1);
    if(!PORT){
        err "PORT malloc()" ner
    }
    strcpy(PORT,argv[optind+1]);

    malloc_connections(N_CONNEXIONS);

    SOCKADDR_IN6 sin6 = bind_socket(&sock, HOSTNAME, PORT);
    char addrstr[100];
    inet_ntop (sin6.sin6_family, &sin6.sin6_addr, addrstr, 100);
    per "Hostname : %s - Port : %d - Nb max connections: %d\n",addrstr, ntohs(sin6.sin6_port),N_CONNEXIONS );

    if(sock == INVALID_SOCKET) {
        err "socket()" ner
    }

    bool ra = true;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &ra, sizeof(ra));

    int wait = exitSec;

    fd_set fdSet;
    TIMEVAL timeout;
    while(exitSec == -1 || wait>0)
    {
        FD_ZERO(&fdSet);
        FD_SET(sock, &fdSet);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ready = select(sock + 1, &fdSet, NULL, NULL, &timeout);
        if (ready < 0) {
			err "select()" ner
		}
		else if(ready == 0 && exitSec!=-1)
        {
            per WHITE"Wait (exit in %d)\n",wait);
            wait--;
        }
		else if(FD_ISSET(sock,&fdSet)){
            handle_reception();
            wait = exitSec;
		}

		check_times_out();
    }

    per "Bye\n");
    free_connections();
    close(sock);
    free(HOSTNAME);
    free(PORT);
    if(FORMAT)
        free(FORMAT);

    return 0;
}
