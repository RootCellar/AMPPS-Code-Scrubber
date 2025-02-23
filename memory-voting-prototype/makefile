CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS =

LINTER=cppcheck --enable=all --inconclusive --max-ctu-depth=4 -j 4 --library=posix

SOURCE=$(wildcard *.c)
OBJECTS=$(SOURCE:%.c=%.o)

.PHONY: all clean test

all: prototype unit_tests demo

clean:
	rm -f prototype
	rm -f $(OBJECTS)

check: all test lint

lint:
	 $(LINTER) .

test: all
	time ./prototype

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

unit_tests: memory_correction.o testing.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ unit_tests.c $^

prototype: memory_correction.o testing.o
	$(CC) $(CFLAGS) -o $@ prototype_voting.c $^

demo: memory_correction.o testing.o util.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ demo.c $^
