CC = gcc
CFLAGS = -O0 -g -Wall
LDFLAGS =

all: neural-network

neural-network: neural-network.o
	$(CC) $(LDFLAGS) neural-network.o -o neural-network

neural-network.o:
	$(CC) $(CFLAGS) -c neural-network.c

clean:
	rm *.o neural-network

.PHONY: clean
