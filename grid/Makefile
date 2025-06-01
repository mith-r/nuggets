# Makefile for Grid Module

OBJS = grid.o file.o 
LIB = grid.a
LIBS = -lm
LLIBS = ../grid/grid.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -c

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

grid.o: grid.h
file.o: file.h

.PHONY: all clean

all: grid

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core