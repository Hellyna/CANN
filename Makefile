CC = gcc
#CFLAGS = -O0 -g -Wall -Wextra -pedantic -Werror -std=c99
CFLAGS = -O2 -pipe -march=native --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2
LDFLAGS = -lm -fopenmp

.PHONY: clean

all: neural-network

neural-network: main.o neural-network.o activation-functions.o error-data.o training.o training-set.o resilient-propagation.o libcsv.o csv.o util.o math.o
	$(CC) $(LDFLAGS) main.o neural-network.o activation-functions.o error-data.o training.o training-set.o resilient-propagation.o libcsv.o csv.o util.o math.o -o neural-network

main.o:
	$(CC) $(CFLAGS) -c main.c

neural-network.o:
	$(CC) $(CFLAGS) -c neural-network.c

activation-functions.o:
	$(CC) $(CFLAGS) -c activation-functions.c

error-data.o:
	$(CC) $(CFLAGS) -c error-data.c

training.o:
	$(CC) $(CFLAGS) -c training.c

training-set.o:
	$(CC) $(CFLAGS) -c training-set.c

resilient-propagation.o:
	$(CC) $(CFLAGS) -c resilient-propagation.c

libcsv.o:
	$(CC) $(CFLAGS) -c libcsv/libcsv.c

csv.o:
	$(CC) $(CFLAGS) -c libcsv/csv.c

util.o:
	$(CC) $(CFLAGS) -c util/util.c

math.o:
	$(CC) $(CFLAGS) -c util/math.c

clean:
	rm *.o neural-network

