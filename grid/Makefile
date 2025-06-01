# Makefile for Grid Module

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I

OBJS = grid.o file.o 
TEST_OBJS = gridTest.o

grid.a: $(OBJS)
	ar cr grid.a $(OBJS)

grid.Test: $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o gridTest $(OBJS) $(TEST_OBJS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: all clean

all: grid.a gridTest

clean:
	rm -f *.o grid.a gridTest