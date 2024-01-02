#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "./gp_random.c"
#include "./gp_game.c"

#define DUMP_FILEPATH "./game.bin"
#define TRAINED_FILEPATH "./trained.bin"
#define MAX_GENERATIONS 10

Game games[2] = {0};

int main(void)
{
    srand(time(0));

    int current = 0;
    init_game(&games[current]);
    load_game(DUMP_FILEPATH, &games[current]);

    for (int i = 0; i < MAX_GENERATIONS; ++i)
    {
        printf("Generation %d... ", i);
        fflush(stdout);

        clock_t begin = clock();
        while (!is_everyone_dead(&games[current])) {
            step_game(&games[current]);
        }
        printf("%fs\n", (float) (clock() - begin) / (float) CLOCKS_PER_SEC);

        int next = 1 - current;
        make_new_generation(&games[current], &games[next]);
        current = next;
    }

    dump_game(TRAINED_FILEPATH, &games[current]);

    return 0;
}