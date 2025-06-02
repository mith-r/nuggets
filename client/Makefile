# Makefile for CS50 Nuggets client module
#
# Authors: Tarini, Ben, Gustavo, Mithun

PROGS = client
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
VAL = valgrind --leak-check=full --show-leak-kinds=all -s
LIB = ../support/message.o ../support/log.o

.PHONY: all clean valgrind

all: client 

client: client.o $(LIB)
	$(CC) $(CFLAGS) $^ -lncurses -o $@

client.o: $(LIB)


test: $(PROGS)
	bash ./testing.sh &> testing.out

clean:
	rm -f *.o
	rm -f vgcore*
	rm -f client