# vim: noet:

C 			=gcc
SRC 		=src/*.c
CFLAGS 		=-Wall -g -O3
OUTPUT 		=./bin/test
LIB 		=-lm
VALGRIND 	=valgrind

all:
	$(C) $(SRC) -o $(OUTPUT) $(CFLAGS) $(LIB)

test: all
	$(OUTPUT)

memcheck: all
	$(VALGRIND) --tool=memcheck --leak-check=yes --show-reachable=yes \
		--num-callers=20 --track-fds=yes $(OUTPUT)
