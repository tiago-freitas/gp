#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 10

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800

#define CELL_WIDTH ((float) SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / BOARD_HEIGHT)

int scc(int code)
{
    if (code < 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }

    return code;
}

void *scp(void *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit(1);
    }

    return ptr;
}

void render_grid_board(SDL_Renderer *renderer)
{
    scc(SDL_SetRenderDrawColor(renderer, 90, 90, 90, 255));
    for (int x = 1; x < BOARD_WIDTH; ++x) {
        scc(SDL_RenderDrawLine(
                renderer,
                CELL_WIDTH * x,
                0,
                CELL_WIDTH * x,
                SCREEN_HEIGHT));
    }
    for (int y = 1; y < BOARD_HEIGHT; ++y) {
        // printf("cell_height = %f, y = %d, cell*y = %f\n", CELL_HEIGHT, y, CELL_HEIGHT*y);
        scc(SDL_RenderDrawLine(
                renderer,
                0,
                CELL_HEIGHT * y,
                SCREEN_WIDTH,
                CELL_HEIGHT * y));
    }

    //     for (int y = 0; y < BOARD_HEIGHT; ++y) {
    //         SDL_RenderDrawLine(renderer,
    //                            int x1, int y1, int x2, int y2);
    //     }
    // }
}

int main(void)
{
    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *const window = scp(SDL_CreateWindow(
        "Hunger Games",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE));

    SDL_Renderer *const renderer = scp(SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    scc(SDL_RenderSetLogicalSize(
            renderer,
            SCREEN_WIDTH,
            SCREEN_HEIGHT));

    int quit = 0;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    quit = 1;
                } break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255));
        scc(SDL_RenderClear(renderer));

        render_grid_board(renderer);

        SDL_RenderPresent(renderer);
 
    }



    SDL_Quit();
    return 0;
}