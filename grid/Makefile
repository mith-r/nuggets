# Makefile for Grid Module

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb 

OBJS = grid.o file.o 
TEST_OBJS = gridTest.o

all: grid.a gridTest

grid.a: $(OBJS)
	ar cr grid.a $(OBJS)

gridTest: $(OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o gridTest $(OBJS) $(TEST_OBJS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: all clean

clean:
	rm -f *.o grid.a gridTest