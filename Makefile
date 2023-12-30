PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=-lm `pkg-config --libs $(PKGS)`

gp: main.c style.h
	$(CC) $(CFLAGS) -o gp main.c $(LIBS)

