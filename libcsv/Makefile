CC = gcc
#CFLAGS = -Wall -O0 -g 
CFLAGS = -O2 -pipe -march=native -fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2
LDFLAGS =

.PHONY: clean

all: csvtest

csvtest: libcsv.o csv.o csvtest.o util.o
	$(CC) $(LDFLAGS) libcsv.o csv.o csvtest.o util.o -o csvtest

util.o:
	$(CC) $(CFLAGS) -c ../util/util.c

csvtest.o:
	$(CC) $(CFLAGS) -c csvtest.c

libcsv.o:
	$(CC) $(CFLAGS) -c libcsv.c

csv.o:
	$(CC) $(CFLAGS) -c csv.c

clean:
	rm *.o csvtest

