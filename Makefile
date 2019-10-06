CC = gcc
CFLAGS += -std=c99
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wshadow
CFLAGS += -Wextra
CFLAGS += -fstack-protector-all

receiver: main.o packet_implem.o
	$(CC) $(CFLAGS) -o receiver main.o packet_implem.o -lz
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
packet_implem.o: packet_implem.c
	$(CC) $(CFLAGS) -lz -c packet_implem.c

all : receiver
