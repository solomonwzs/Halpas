# vim: noet:

C 		=gcc
SRC 	=src/*.c
CFLAGS 	=-Wall -g -O
OUTPUT 	=./bin/test
LIB 	=-lm

all:
	$(C) $(SRC) -o $(OUTPUT) $(CFLAGS) $(LIB)

test: all
	$(OUTPUT)
