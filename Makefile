CFLAGS = -fsanitize-recover=address,undefined -Wswitch -Wall -Wextra -g

all: src/lox.c
	cc $(CFLAGS) -o lox src/lox.c
