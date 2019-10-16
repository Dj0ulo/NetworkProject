CC = gcc
CFLAGS += -std=gnu99
CFLAGS += -Wall
CFLAGS += -Werror
CFLAGS += -Wshadow
CFLAGS += -Wextra
CFLAGS += -fstack-protector-all

receiver: main.o packet_implem.o socket.o utils.o
	$(CC) $(CFLAGS) -o receiver main.o packet_implem.o -lz socket.o utils.o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
packet_implem.o: packet_implem.c
	$(CC) $(CFLAGS) -lz -c packet_implem.c
socket.o: socket.c
	$(CC) $(CFLAGS) -c socket.c
utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

all : receiver
