test: main.o packet.o
	gcc -o test main.o packet.o -lz -lcunit

main.o: main.c
	gcc -c main.c
packet.o: ../src/packet.c
	gcc -c ../src/packet.c
