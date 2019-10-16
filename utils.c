#include "utils.h"

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
    if(!pkt)
        printf("(NULL)\n");
    else{
        printf("type : %d\n", pkt_get_type(pkt));
        printf("tr : %d\n", pkt_get_tr(pkt));
        printf("window : %d\n", pkt_get_window(pkt));
        printf("length : %d\n", pkt_get_length(pkt));
        printf("seqnum : %d\n", pkt_get_seqnum(pkt));
        printf("timestamp : %x\n", pkt_get_timestamp(pkt));
        printf("crc1 : 0x%x\n", pkt_get_crc1(pkt));
        printf("payload : %s\n", pkt_get_payload(pkt));
        printf("crc2 : 0x%x\n", pkt_get_crc2(pkt));
    }
    printf("--------------\n\n");
}
void print_sockaddr_in6(const SOCKADDR_IN6 *sin6)
{
    printf("--Print sockaddr_in6---\n");
    if(!sin6)
        printf("(NULL)\n");
    else{
        printf("family : %d\n", sin6->sin6_family);
        printf("port : %d\n", sin6->sin6_port);
        printf("flowinfo : %d\n", sin6->sin6_flowinfo);
        printf("sin6_addr. : %d\n", sin6->sin6_flowinfo);
        char addrstr[100];
        inet_ntop (sin6->sin6_family, &sin6->sin6_addr, addrstr, 100);
        printf ("IPv%d address: %s\n", sin6->sin6_family == PF_INET6 ? 6 : 4,addrstr);
        printf("scope_id : %d\n", sin6->sin6_scope_id);
    }
    printf("--------------\n\n");
}
