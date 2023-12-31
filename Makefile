PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wextra -Wold-style-definition -ggdb -std=c11 -pedantic `pkg-config --cflags $(PKGS)`
LIBS=-lm `pkg-config --libs $(PKGS)`

gp: $(wildcard src/gp*)
	$(CC) $(CFLAGS) -o gp src/gp.c $(LIBS) -D_XOPEN_SOURCE

