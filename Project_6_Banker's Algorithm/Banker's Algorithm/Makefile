# makefile for thread pool
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: banker.o
	$(CC) $(CFLAGS) -o banker banker.o $(PTHREADS)

banker.o: banker.c
	$(CC) $(CFLAGS) -c banker.c $(PTHREADS)


clean:
	rm -rf *.o
	rm -rf banker

