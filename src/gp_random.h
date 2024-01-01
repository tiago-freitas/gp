#ifndef GP_RANDOM_H_
#define GP_RANDOM_H_

int random_int_range(int low, int high);

Coord random_coord_on_board(void);

Coord random_empty_coord_on_board(const Game *game);

Env random_env(void);

Action random_action(void);

Dir random_dir(void);

#endif // GP_RANDOM_H_