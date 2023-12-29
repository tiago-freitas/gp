PKGS=sdl2
CFLAGS=-Wall -Wextra -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2`

gp: main.c triangle.c style.h triangle.h 
	$(CC) $(CFLAGS) -o gp main.c triangle.c $(LIBS)

