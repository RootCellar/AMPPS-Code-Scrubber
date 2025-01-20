CC = eg++
CFLAGS = -g -Wall -Wextra
LDFLAGS =

LINTER=cppcheck --enable=all --inconclusive --max-ctu-depth=4 -j 4 --library=posix

.PHONY: all clean test

all: prototype

clean:
	rm -f prototype

check: all test lint

lint:
	 $(LINTER) .

test: all
	time ./prototype

prototype:
	$(CC) $(CFLAGS) -o $@ prototype-voting.c
