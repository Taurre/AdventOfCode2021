CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -fno-common -g

.PHONY: clean all

all: part-1 part-2

part-1: part-1.o
part-2: part-2.o
part-2.o: cost.h
cost.h: cost
	./cost >cost.h
cost: cost.o
clean:
	rm -f *.o part-1 part-2 cost.h cost.o cost
