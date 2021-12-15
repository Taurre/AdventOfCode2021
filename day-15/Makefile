CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -fno-common -g
LDFLAGS = -lm

.PHONY: clean all

all: part-1 part-2 big_map

part-1: part-1.o
part-2: part-2.o
big_map: big_map.o
clean:
	rm -f *.o part-1 part-2 big_map
