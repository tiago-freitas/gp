#ifndef GP_GAME_H_
#define GP_GAME_H_

#define BOARD_WIDTH  100
#define BOARD_HEIGHT 100

#define ATTACK_DAMAGE        10
#define HEALTH_MAX           100
#define FOOD_HUNGER_RECOVERY 10
#define HUNGER_MAX           100
#define STEP_HUNGER_DAMAGE   5

#define AGENTS_COUNT 2000
#define FOODS_COUNT  1000
#define WALLS_COUNT  200
#define GENES_COUNT  10
#define STATE_COUNT  5

static_assert(AGENTS_COUNT + FOODS_COUNT + WALLS_COUNT <= BOARD_WIDTH * BOARD_HEIGHT,
              "Too many entities");

typedef struct {
    int x, y;
} Coord;

typedef enum {
    DIR_RIGHT = 0,
    DIR_UP,
    DIR_LEFT,
    DIR_DOWN,
    DIR_COUNT
} Dir;

Coord coord_dirs[4] = {
    // DIR_RIGHT
    {1, 0},
    // DIR_UP,
    {0, -1},
    // DIR_LEFT,
    {-1, 0},
    // DIR_DOWN,
    {0, 1},
};

typedef int State;

typedef enum {
    ENV_NOTHING = 0,
    ENV_AGENT,
    ENV_FOOD,
    ENV_WALL,
    ENV_COUNT,
} Env;

typedef enum {
    ACTION_NOP = 0,
    ACTION_STEP,
    ACTION_TURN_LEFT,
    ACTION_TURN_RIGHT,
    ACTION_COUNT,
} Action;

typedef struct {
    State state;
    Env env;
    Action action;
    State next_state;
} Gene;

typedef struct {
    size_t count;
    Gene genes[GENES_COUNT];
} Chromo;

typedef struct {
    Coord pos;
    Dir dir;
    int hunger;
    int health;
    State state;
    int lifetime;
    Chromo chromo;
} Agent;

const char *dir_as_cstr(Dir dir);
void print_agent(FILE *stream, const Agent *agent);

typedef struct {
    int eaten;
    Coord pos;
} Food;

typedef struct {
    Coord pos;
} Wall;

typedef struct {
    Agent agents[AGENTS_COUNT];
    Food foods[FOODS_COUNT];
    Wall walls[WALLS_COUNT];
} Game;

const char *env_as_cstr(Env env);
const char *action_as_cstr(Action action);
int coord_equals(Coord a, Coord b);
void print_chromo(FILE *stream, const Chromo *chromo);
int is_cell_occupy(const Game *game, Coord pos);

void init_game(Game *game);

Agent *agent_at(Game *game, Coord pos);
Coord coord_infront_of_agent(const Agent *agent);
Food *food_infront_of_agent(Game *game, size_t agent_index);
Agent *agent_infront_of_agent(Game *game, size_t agent_index);
Wall *wall_infront_of_agent(Game *game, size_t agent_index);

Env env_of_agent(Game *game, size_t agent_index);

void step_agent(Agent *agent);
void execute_action(Game *game, size_t agent_index, Action action);
void step_game(Game *game);

int compare_agents_lifetimes(const void *a, const void *b);
void print_best_agents(FILE *stream, Game *game, size_t n);

#endif // GP_GAME_H_