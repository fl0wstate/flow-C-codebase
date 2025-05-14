cc = gcc
flags = -Wall -Werror -pedantic -std=c99 -g
files = ./arena.c ./main.c ./log.c
obj = $(files:.c=.o)

all: binary

binary: $(obj) codebase.h
	$(cc) $(flags) $(obj) -o $@

clean:
	rm -rf $(obj) binary $(wildcard ./*.out)

.PHONY: all clean binary
