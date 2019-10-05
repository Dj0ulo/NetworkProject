#include <stdio.h>
#include <stdlib.h>

#include "packet_interface.h"

bool getBit(uint16_t n, uint8_t index){
    return !!(n & (1u << index));
}

void printBits(uint16_t n){
    printf("%d : ",n);
    for(uint8_t i=0;i<16;i++)
    {
        printf("%d",getBit(n,i));
        if((i-3)%4 == 0)
            printf(" ");
    }
    printf("\n");
}

int main()
{
    printf("Hello world!\n");
    const uint16_t x = 3012;
    printBits(0xe1);
    printBits(0x5c);



    uint8_t *data = malloc(2);
    printf("%u, size %u\n",x, varuint_encode(x,data,2) );
    printBits(*(uint16_t*)data);
    uint16_t ret;
    printf("size %u\n",varuint_decode(data,2,&ret));
    printBits(ret);

    free(data);
    return 0;
}
