CC = gcc
CFLAGS = -Wall -fstack-protector

cclient: cclient.c cclient.h interface.h interface.o
	$(CC) $(CFLAGS) interface.o -o $@ $@.c

server: server.c server.h interface.h interface.o
	$(CC) $(CFLAGS) interface.o -o $@ $@.c

interface:
	$(CC) $(CFLAGS) -o $@.o

all: cclient server interface.o

clean:
	-rm -f cclient server interface.o
