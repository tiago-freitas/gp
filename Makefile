PKGS=sdl2 SDL2_gfx
CFLAGS=-Wall -Wextra -Wold-style-definition -ggdb -std=c11 -pedantic
CFLAGS_SDL=`pkg-config --cflags $(PKGS)`
LIBS=-lm
LIBS_SDL=`pkg-config --libs $(PKGS)`

COMMON_SRC=src/gp_game.h src/gp_game.c src/gp_random.c src/gp_random.h

all: gp_trainer gp_simulator

gp_trainer: src/gp_trainer.c $(COMMON_SRC)
	$(CC) $(CFLAGS) -o gp_trainer src/gp_trainer.c $(LIBS)

gp_simulator: src/gp_simulator.c $(COMMON_SRC) src/gp_visual.h src/gp_visual.c
	$(CC) $(CFLAGS) $(CFLAGS_SDL) -o gp_simulator src/gp_simulator.c $(LIBS) $(LIBS_SDL) -D_XOPEN_SOURCE

