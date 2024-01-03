#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./gp_random.c"
#include "./gp_game.c"
#include "./gp_visual.c"

#define DUMP_FILEPATH "./game.bin"
#define TRAINED_FILEPATH "./trained.bin"
#define MAX_GENERATIONS 10

const char *shift(int *argc, const char ***argv)
{
    assert(*argc > 0);
    const char *result = **argv;
    *argc -= 1;
    *argv += 1;
    return result;
}

void usage(FILE *stream)
{
    fprintf(stream, "./gp_simulator <input.bin>\n");
}
Game games[2] = {0};

int main(int argc, const char *argv[])
{
    shift(&argc, &argv);

    if (argc == 0) {
        usage(stderr);
        fprintf(stderr, "ERROR: input filepath is not provided\n");
        exit(1);
    }
    const char *input_filepath = shift(&argc, &argv);

    putenv((char *)"SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR=0");
    
    scc(SDL_Init(SDL_INIT_VIDEO));
   
    SDL_Window *const window = scp(SDL_CreateWindow(
        "Hunger Games",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE));

    SDL_Renderer *const renderer = scp(SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED));

    // SDL_RENDERER_PRESENTVSYNC

    scc(SDL_RenderSetLogicalSize(
            renderer,
            SCREEN_WIDTH,
            SCREEN_HEIGHT));

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");


    int quit = 0;
    int current = 0;
    load_game(input_filepath, &games[current]);

    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

            case SDL_QUIT: {
                quit = 1;
            } break;

            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                case SDLK_SPACE: {
                    step_game(&games[current]);
                } break;

                case SDLK_p: {
                    print_best_agents(stdout, &games[current], SELECTION_POOL);
                } break;

                // case SDLK_r: {
                //     init_game(&games[current]);
                // } break;


                // case SDLK_n: {
                //     int next = 1 - current; // flip flop 1 | 0
                //     make_new_generation(&games[current], &games[next]);
                //     current = next;
                // } break;

                // case SDLK_d: {
                //     dump_game(DUMP_FILEPATH, &games[current]);
                // } break;
                
                } 
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                switch (event.button.button) {
                case SDL_BUTTON_LEFT: {
                    Coord pos;
                    pos.x = (int) floorf(event.button.x / CELL_WIDTH);
                    pos.y = (int) floorf(event.button.y / CELL_HEIGHT);
                    Agent *agent = agent_at(&games[current], pos);
                    if (agent) {
                        print_agent(stdout, agent);
                    }
                } break;
                }
            } break;

            }
        }

        scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
        scc(SDL_RenderClear(renderer));

        // render_grid_board(renderer);
        render_game(renderer, &games[current]);

        SDL_RenderPresent(renderer);

        // Cap framerate!
        // SDL_Delay(floor(16.666f));
        SDL_Delay(30.0f);
    }

    SDL_Quit();
    return 0;
}