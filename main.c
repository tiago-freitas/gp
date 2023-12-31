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

#define FOODS_COUNT 4
#define FOOD_PADDING (AGENT_PADDING)

#define WALLS_COUNT 4

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

int swap_hex(Uint32 hex)
{
    int r = (hex & 0xFF) << 24;
    int g = ((hex >> 8) & 0xFF)  << 16;
    int b = ((hex >> 16) & 0xFF) << 8;
    int a = ((hex >> 24));

    return r + g + b + a;
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

typedef int State;

typedef enum {
    ENV_NOTHING = 0,
    ENV_AGENT,
    ENV_FOOD,
    ENV_WALL,
} Env;

typedef enum {
    ACTION_NOP = 0,
    ACTION_STEP,
    ACTION_EAT,
    ACTION_ATTACK,
} Action;

typedef struct {
    State state;
    Env env;
    Action action;
    State next_state;
} Brain_Cell;

typedef struct {
    size_t count;
    Brain_Cell cells[];
} Brain;

typedef struct {
    int pos_x, pos_y;
    Dir dir;
    int hunger;
    int health;
    State state;
} Agent;


typedef struct {
    int eaten;
    int pos_x;
    int pos_y;
} Food;

typedef struct {
    int pos_x;
    int pos_y;
} Wall;

typedef struct {
    Agent agents[AGENTS_COUNT];
    Food foods[FOODS_COUNT];
    Wall walls[WALLS_COUNT];
} Game;

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

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    // scale and shift
    Sint16 x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos_x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos_y * CELL_HEIGHT + AGENT_PADDING;

    Uint32 c = swap_hex(AGENT_COLOR);

    filledTrigonColor(renderer, x1, y1, x2, y2, x3, y3, c);
    aatrigonColor(renderer, x1, y1, x2, y2, x3, y3, c);
}

void render_wall(SDL_Renderer *renderer, Wall wall)
{
    const SDL_Rect rect = {
        (int) floorf(wall.pos_x * CELL_WIDTH),
        (int) floorf(wall.pos_y * CELL_HEIGHT),
        (int) floorf(CELL_WIDTH),
        (int) floorf(CELL_HEIGHT),
    };

    sdl_set_color_hex(renderer, WALL_COLOR);

    SDL_RenderFillRect(renderer, &rect);
}

void render_food(SDL_Renderer *renderer, Food food)
{
    Sint16 x = floorf(food.pos_x * CELL_WIDTH  + (CELL_WIDTH  * 0.5f));
    Sint16 y = floorf(food.pos_y * CELL_HEIGHT + (CELL_HEIGHT * 0.5f));
    Sint16 rad = floorf(fminf(CELL_WIDTH, CELL_HEIGHT) * 0.5f - FOOD_PADDING);

    Uint32 c = swap_hex(FOOD_COLOR);
    
    aacircleColor(renderer, x, y, rad, c);
    filledCircleColor(renderer, x, y, rad, c);
}

void render_game(SDL_Renderer *renderer, const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        render_agent(renderer, game->agents[i]);
    }        

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        render_wall(renderer, game->walls[i]);
    }        

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        render_food(renderer, game->foods[i]);
    }        
}

void init_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i] = random_agent();
        game->agents[i].dir = i;
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        game->foods[i].pos_x = random_int_range(0, BOARD_WIDTH);
        game->foods[i].pos_y = random_int_range(0, BOARD_HEIGHT);
    }

    for (size_t i = 0; i < WALLS_COUNT ; ++i) {
        game->walls[i].pos_x = random_int_range(0, BOARD_WIDTH);
        game->walls[i].pos_y = random_int_range(0, BOARD_HEIGHT);
    }
}

Agent agents[AGENTS_COUNT];

void step_game(Game *game)
{
    // TODO
}

Game game = {0};

int main(void)
{
    init_game(&game);

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


    int quit = 0;
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
                    step_game(&game);
                } break;
                }
            } break;
            }
        }

        sdl_set_color_hex(renderer, BACKGROUND_COLOR);
        scc(SDL_RenderClear(renderer));

        render_grid_board(renderer);
        render_game(renderer, &game);

        SDL_RenderPresent(renderer);
        // quit = 1;
    }

    SDL_Quit();
    return 0;
}