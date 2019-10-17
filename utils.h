#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "packet_interface.h"
#include "socket.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[0m"

typedef uint8_t bool;
#define true 1
#define false 0
#define bytearray char*

#define NB_MAX_CHAR_UINT32 10 //2 ^ 32 = 4,294,967,296 => 10 digits

bool getBit(uint16_t n, uint8_t index);
void printBits(uint16_t n);
void printHex(const uint8_t *bytes, size_t len);
void printPkt(const pkt_t* pkt);
void print_sockaddr_in6(const SOCKADDR_IN6 *sin6);

#endif // UTILS_H_INCLUDED
