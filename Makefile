# Makefile for nuggets
# 

OBJS = server.o
LIBS = support

CFLAGS = -Wall -pedantic -std=gnu11 -ggdb -I../libcs50 -I../common
CC = gcc
MAKE = make

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o server


.PHONY: all test clean

../common/common.a:
	cd ../common
	make clean
	make

# test: servertest
# 	chmod +x ./testing.sh
# 	bash -v ./testing.sh &> testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f client
	rm -f core
