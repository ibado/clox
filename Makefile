all: src/lox.c
	cc -Wswitch-enum -Wall -Wextra -g -o lox src/lox.c
