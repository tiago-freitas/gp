#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

#include "./style.h"

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 10

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800

#define CELL_WIDTH ((float) SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / BOARD_HEIGHT)

#define AGENTS_COUNT 4
#define AGENT_PADDING 15.0f

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

void sdl_set_color_hex(SDL_Renderer *renderer, Uint32 hex)
{
    int r =  hex >> 24;
    int g = (hex >> 16) & 0xFF;
    int b = (hex >> 8) & 0xFF;
    int a =  hex & 0xFF;

    scc(SDL_SetRenderDrawColor(
            renderer,
            r, g, b, a));
}

typedef enum {
    DIR_RIGHT = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
} Dir;

float agents_dirs[4][6] = {
    // DIR_RIGHT
    {0.0, 0.0, 1.0, 0.5, 0.0, 1.0},
    //DIR_UP
    {0.0, 1.0, 0.5, 0.0, 1.0, 1.0},
    // DIR_LEFT
    {1.0, 0.0, 1.0, 1.0, 0.0, 0.5},
    //DIR_DOWN,    
    {0.0, 0.0, 1.0, 0.0, 0.5, 1.0},
};

typedef struct {
    int pos_x, pos_y;
    Dir dir;
    int hunger;
    int health;
} Agent;

typedef enum {
    ACTION_NOP = 0,
    ACTION_STEP,
    ACTION_EAT,
    ACTION_ATTACK,
} Action;

Agent agents[AGENTS_COUNT];

void render_grid_board(SDL_Renderer *renderer)
{
    sdl_set_color_hex(renderer, GRID_COLOR);
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
}

int random_int_range(int low, int high)
{
    return rand() % (high - low) + low;
}

Dir random_dir(void)
{
    return (Dir) random_int_range(0, 4);
}

Agent random_agent(void)
{
    Agent agent  = {0};
    agent.pos_x  = random_int_range(0, BOARD_WIDTH);
    agent.pos_y  = random_int_range(0, BOARD_HEIGHT);
    agent.dir    = random_dir();
    agent.hunger = 100;
    agent.health = 100;

    return agent;
}

void init_agents(void)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        agents[i] = random_agent();
        agents[i].dir = i;
    }
}


void render_agent(SDL_Renderer *renderer, Agent agent)
{


    // scale and shift
    Sint16 x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;

    filledTrigonColor(renderer, x1, y1, x2, y2, x3, y3, AGENT_COLOR);
    aatrigonColor(renderer, x1, y1, x2, y2, x3, y3, AGENT_COLOR);
}

void render_all_agents(SDL_Renderer *renderer)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        render_agent(renderer, agents[i]);
    }
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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    init_agents();

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

        sdl_set_color_hex(renderer, BACKGROUND_COLOR);
        scc(SDL_RenderClear(renderer));

        render_grid_board(renderer);
        render_all_agents(renderer);

        SDL_RenderPresent(renderer);
        // quit = 1;
    }

    SDL_Quit();
    return 0;
}