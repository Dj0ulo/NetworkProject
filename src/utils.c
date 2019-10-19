#include "utils.h"
#include <unistd.h>


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
int write_bytes(int fd, const char* bytes, const size_t size)
{
    for(size_t off=0;off<size;)
    {
        int n = write(fd, bytes+off, size);
        if(n==-1)
            return -1;
        off+=n;
    }
    return size;
}

time_t start = 0;
void startClock()
{
    start = millis();
}
time_t millis()
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (tv.tv_sec%1000000)*1000 + tv.tv_usec/1000 - start;
}
