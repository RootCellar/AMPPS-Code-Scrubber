CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
LDFLAGS =

LINTER=cppcheck --enable=all --inconclusive --max-ctu-depth=4 -j 4 --library=posix

SOURCE=$(wildcard *.c)
HEADERS=$(wildcard *.h)

OBJECTS=$(SOURCE:%.c=%.o)
ASSEMBLIES=$(SOURCE:%.c=%.asm)

EXECUTABLES=prototype unit_tests demo

.PHONY: all everything clean check lint assemblies objects executables run_tests run_demo

all: executables
everything: assemblies objects executables

clean:
	rm -f $(EXECUTABLES)
	rm -f $(OBJECTS)
	rm -f $(ASSEMBLIES)

check: all run_tests lint

lint:
	 $(LINTER) .

run_tests: unit_tests
	./unit_tests

run_demo: demo
	./demo

assemblies: $(ASSEMBLIES)
objects: $(OBJECTS)
executables: $(EXECUTABLES)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.asm: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

unit_tests: memory_correction.o environment.o testing.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ unit_tests.c $^

prototype: memory_correction.o environment.o testing.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ prototype_voting.c $^

demo: memory_correction.o environment.o testing.o util.o terminal_color.o
	$(CC) $(CFLAGS) -o $@ demo.c $^
