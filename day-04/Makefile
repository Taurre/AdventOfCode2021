CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -fno-common -g

.PHONY: clean all

all: part-1 part-2

part-1: part-1.o list.o
part-2: part-2.o list.o
clean:
	rm -f *.o part-1 part-2
