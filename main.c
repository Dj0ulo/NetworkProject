#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "packet_interface.h"

bool getBit(uint16_t n, uint8_t index){
    return !!(n & (1u << index));
}

void printBits(uint16_t n){
    printf("%d (0x%x) : ",n,n);
    for(uint8_t i=0;i<16;i++)
    {
        printf("%d",getBit(n,i));
        if((i-3)%4 == 0)
            printf(" ");
    }
    printf("\n");
}
void printHex(const uint8_t *bytes, size_t len){
    for(size_t i=0;i<len;i++)
        printf("%x ",bytes[i]);
    printf("\n");
}
void printPkt(const pkt_t* pkt){
    printf("---Print packet---\n");
    printf("type : %d\n", pkt_get_type(pkt));
    printf("tr : %d\n", pkt_get_tr(pkt));
    printf("window : %d\n", pkt_get_window(pkt));
    printf("length : %d\n", pkt_get_length(pkt));
    printf("seqnum : %d\n", pkt_get_seqnum(pkt));
    printf("timestamp : %d\n", pkt_get_timestamp(pkt));
    printf("crc1 : 0x%x\n", pkt_get_crc1(pkt));
    printf("payload : %s\n", pkt_get_payload(pkt));
    printf("crc2 : 0x%x\n", pkt_get_crc2(pkt));
    printf("--------------\n");
}

int main(int argc, char *argv[])
{
    char *FORMAT = NULL, *HOSTNAME = NULL;
    uint32_t PORT = 0;
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
	if(!HOSTNAME) {
        fprintf(stderr,"ERROR allocating HOSTNAME\n"); return -1;
    }
    strcpy(HOSTNAME, argv[optind]);
    PORT = atoi(argv[optind+1]);


    printf("Hello world!\n");
    printf("%s %d - N: %d\n",HOSTNAME, PORT, N_CONNEXIONS);

    //const uint16_t x = 0x8000-1;

    printBits(0x5c);
    printBits((0x5c & 0b11000000) >> 6);

    pkt_t * pkt = pkt_new();
    pkt_set_payload(pkt,"hello world",11);
    pkt_set_window(pkt,28);
    pkt_set_seqnum(pkt,0x7b);
    pkt_set_timestamp(pkt, 0x17);


    char buf[MAX_PACKET_SIZE];
    size_t len = MAX_PACKET_SIZE;
    printf("%d",pkt_encode(pkt, buf, &len));
    printPkt(pkt);
    printHex((uint8_t*)buf, len);

    pkt_t *pRec = pkt_new();
    printf("%d",pkt_decode(buf, len,pRec));
    printPkt(pRec);

//    uint8_t *data = malloc(2);
//    printf("%u, size %ld\n",x, varuint_encode(x,data,2) );
//    printBits(*(uint16_t*)data);
//    uint16_t ret;
//    printf("size %ld\n",varuint_decode(data,2,&ret));
//    printBits(ret);
//    free(data);
    pkt_del(pRec);
    pkt_del(pkt);
    free(HOSTNAME);
    free(FORMAT);
    return 0;
}
