#include "./gp_game.h"

int is_cell_occupy(const Game *game, Coord pos)
{
    // printf("%s\n", env_as_cstr(game->gameboard[pos.x][pos.y].type));
    if (game->gameboard[pos.x][pos.y].type == ENV_NOTHING)
        return 0;
    return 1;
}

int random_int_range(int low, int high)
{
    return rand() % (high - low) + low;
}

Coord random_coord_on_board(void)
{
    Coord result;
    result.x  = random_int_range(0, BOARD_WIDTH);
    result.y  = random_int_range(0, BOARD_HEIGHT);
    return result;
}

Coord random_empty_coord_on_board(const Game *game)
{
    Coord result = random_coord_on_board();
    while (is_cell_occupy(game, result))
        result = random_coord_on_board();
    return result;
}

Env random_env(void)
{
    return random_int_range(0, ENV_COUNT);
}

Action random_action(void)
{
    return random_int_range(0, ACTION_COUNT);
}

Dir random_dir(void)
{
    return (Dir) random_int_range(0, 4);
}