# Makefile for BF parser
# Author: Cody Harrington
# Date: 4/6/2013

CC=gcc
CFLAGS=-ansi -Wall -Wextra -Werror -pedantic -pedantic-errors
SOURCES=bf.c
EXECUTABLE=bf

all: $(EXECUTABLE) clean

$(EXECUTABLE): bf.o
	$(CC) $(CFLAGS) bf.o -o $(EXECUTABLE)

bf.o: $(SOURCES)
	$(CC) $(CFLAGS) -c $(SOURCES)

clean:
	rm -f *.o a.out
