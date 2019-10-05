#include <stdio.h>
#include <stdlib.h>

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

int main()
{
    printf("Hello world!\n");
    const uint16_t x = 0x8000-1;

    printBits(0x16);
    printBits(0x0b);

    pkt_t * pkt = pkt_new();
    pkt_set_payload(pkt,"hello world",11);
    pkt_set_window(pkt,28);

    uint8_t buf[MAX_PACKET_SIZE];
    size_t len = MAX_PACKET_SIZE;
    pkt_encode(pkt, buf, &len);
    printBits(buf[0]);
    printHex(buf, len);
    //pkt_decode(buf)


    uint8_t *data = malloc(2);
    printf("%u, size %u\n",x, varuint_encode(x,data,2) );
    printBits(*(uint16_t*)data);
    uint16_t ret;
    printf("size %u\n",varuint_decode(data,2,&ret));
    printBits(ret);
    free(data);

    pkt_del(pkt);
    return 0;
}
