#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "packet_interface.h"
#include "socket.h"

bool getBit(uint16_t n, uint8_t index);
void printBits(uint16_t n);
void printHex(const uint8_t *bytes, size_t len);
void printPkt(const pkt_t* pkt);
void print_sockaddr_in6(const SOCKADDR_IN6 *sin6);

#endif // UTILS_H_INCLUDED
