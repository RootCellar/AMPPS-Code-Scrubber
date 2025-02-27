CC = gcc
CFLAGS = -g -Wall -Wextra
LDFLAGS =

LINTER=cppcheck --enable=all --inconclusive --max-ctu-depth=4 -j 4 --library=posix

SOURCE=$(wildcard *.c)
HEADERS=$(wildcard *.h)
OBJECTS=$(SOURCE:%.c=%.o)
EXECUTABLES=prototype unit_tests demo

.PHONY: all clean check lint run_tests run_demo

all: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES)
	rm -f $(OBJECTS)

check: all run_tests lint

lint:
	 $(LINTER) .

run_tests: unit_tests
	./unit_tests

run_demo: demo
	./demo

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

unit_tests: memory_correction.o environment.o testing.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ unit_tests.c $^

prototype: memory_correction.o environment.o testing.o
	$(CC) $(CFLAGS) -o $@ prototype_voting.c $^

demo: memory_correction.o environment.o testing.o util.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ demo.c $^
