#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <stdint.h> /* uintx_t */
#include <stdio.h>  /* ssize_t */
#include <sys/time.h>

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

#define MAX_FILENAME 260

#define per fprintf(stderr,
#define prt if(doPrint) printf
#define err fprintf(stderr, RED "[Error] "
#define ne WHITE"\n");
#define ner ne; return -1;

#define NB_MAX_CHAR_UINT32 10 //2 ^ 32 = 4,294,967,296 => 10 digits

extern bool doPrint;

/*
 *  Utilitaires de débogage
 */
bool getBit(uint16_t n, uint8_t index);
void printBits(uint16_t n);
void printHex(const uint8_t *bytes, size_t len);

/*
 *  Ecrit l'integralité d'un tableau d'octet (bytes) de taille size dans le fichier fd
 *  @return la taille si tout s'est bien déroulé, -1 sinon
 */
int write_bytes(int fd, const char* bytes, const size_t size);
void startClock();
time_t millis();

#endif // UTILS_H_INCLUDED
