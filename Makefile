CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic
LIBS=

gp: main.c
	$(CC) $(CFLAGS) -o gp main.c $(LIBS)