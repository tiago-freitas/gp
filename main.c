#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL2/SDL.h>

#define BOARD_WIDTH  10
#define BOARD_HEIGHT 10

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 800

#define CELL_WIDTH ((float) SCREEN_WIDTH / BOARD_WIDTH)
#define CELL_HEIGHT ((float) SCREEN_HEIGHT / BOARD_HEIGHT)

#define AGENTS_COUNT 5

#define BACKGROUND_COLOR "181818ff"
#define GRID_COLOR "5a5a5aff"
#define AGENT_COLOR "5f0910ff"

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

Uint8 hex_to_dec(char x)
{
    if ('0' <= x && x <= '9') return x - '0';
    if ('a' <= x && x <= 'f') return x - 'a' + 10;
    if ('A' <= x && x <= 'F') return x - 'A' + 10;
    printf("ERROR: Incorrect hex character %c\n", x);
    exit(1);
}

Uint8 parse_hex_byte(const char *byte_hex)
{
    return hex_to_dec(*byte_hex) * 0x10 + hex_to_dec(*(byte_hex + 1));
}

void sdl_set_color_hex(SDL_Renderer *renderer, const char *hex)
{
    size_t hex_len  = strlen(hex);
    assert(hex_len == 8);
    scc(SDL_SetRenderDrawColor(
            renderer,
            parse_hex_byte(hex),
            parse_hex_byte(hex + 2),
            parse_hex_byte(hex + 4),
            parse_hex_byte(hex + 6)));
}

typedef enum {
    DIR_RIGHT = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
} Dir;

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
    }
}

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    SDL_SetRenderDrawColor(renderer, AGENT_COLOR);
    
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

        SDL_RenderPresent(renderer);
 
    }

    SDL_Quit();
    return 0;
}