#include "./gp_visual.h"

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
    scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(GRID_COLOR)));

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

void render_agent(SDL_Renderer *renderer, Agent agent)
{
    // scale and shift
    Sint16 x1 = agents_dirs[agent.dir][0] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y1 = agents_dirs[agent.dir][1] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x2 = agents_dirs[agent.dir][2] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y2 = agents_dirs[agent.dir][3] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;
    Sint16 x3 = agents_dirs[agent.dir][4] * (CELL_WIDTH  - AGENT_PADDING * 2) + agent.pos.x * CELL_WIDTH + AGENT_PADDING;
    Sint16 y3 = agents_dirs[agent.dir][5] * (CELL_HEIGHT - AGENT_PADDING * 2) + agent.pos.y * CELL_HEIGHT + AGENT_PADDING;

    Uint32 c = REV(AGENT_COLOR);

    filledTrigonColor(renderer, x1, y1, x2, y2, x3, y3, c);
    aatrigonColor(renderer, x1, y1, x2, y2, x3, y3, c);
}

void render_wall(SDL_Renderer *renderer, Wall wall)
{
    const SDL_Rect rect = {
        (int) floorf(wall.pos.x * CELL_WIDTH),
        (int) floorf(wall.pos.y * CELL_HEIGHT),
        (int) floorf(CELL_WIDTH),
        (int) floorf(CELL_HEIGHT),
    };

    scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(WALL_COLOR)));
    
    SDL_RenderFillRect(renderer, &rect);
}

void render_food(SDL_Renderer *renderer, Food food)
{
    Sint16 x = floorf(food.pos.x * CELL_WIDTH  + (CELL_WIDTH  * 0.5f));
    Sint16 y = floorf(food.pos.y * CELL_HEIGHT + (CELL_HEIGHT * 0.5f));
    Sint16 rad = floorf(fminf(CELL_WIDTH, CELL_HEIGHT) * 0.5f - FOOD_PADDING);

    Uint32 c = REV(FOOD_COLOR);
    
    aacircleColor(renderer, x, y, rad, c);
    filledCircleColor(renderer, x, y, rad, c);
}

void render_game(SDL_Renderer *renderer, const Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (game->agents[i].health > 0)
            render_agent(renderer, game->agents[i]);
    }        

    for (size_t i = 0; i < WALLS_COUNT; ++i) {
        render_wall(renderer, game->walls[i]);
    }        

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        if (!game->foods[i].eaten)
            render_food(renderer, game->foods[i]);
    }        
}