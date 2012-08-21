CC = gcc
CFLAGS = -O0 -g -Wall
LDFLAGS = -lm

.PHONY: clean

all: neural-network

neural-network: neural-network.o libcsv.o csv.o util.o
	$(CC) $(LDFLAGS) neural-network.o libcsv.o csv.o util.o -o neural-network

neural-network.o:
	$(CC) $(CFLAGS) -c neural-network.c

libcsv.o:
	$(CC) $(CFLAGS) -c libcsv/libcsv.c

csv.o:
	$(CC) $(CFLAGS) -c libcsv/csv.c

util.o:
	$(CC) $(CFLAGS) -c util/util.c

clean:
	rm *.o neural-network

