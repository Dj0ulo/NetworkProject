test: main.o packet.o utils.o
	gcc -o test main.o packet.o -lz -lcunit utils.o

main.o: main.c
	gcc -c main.c
packet.o: ../src/packet.c
	gcc -c ../src/packet.c
utils.o: ../src/utils.c
	gcc -c ../src/utils.c
