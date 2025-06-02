# Makefile for nuggets
# 

OBJS = server.o
LIBS =  ../grid/grid.o ../grid/file.o ../support/message.o ../support/log.o

CFLAGS = -Wall -pedantic -std=gnu11 -ggdb -Isupport
CC = gcc
MAKE = make

all: server

server: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o server -lm


.PHONY: all test clean

support/support.a:
	$(MAKE) -C support

test:
	chmod +x ./testing.sh
	bash ./testing.sh &> testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f core
