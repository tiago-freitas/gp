#include "./gp_game.h"
#include "./gp_random.c"

int coord_equals(Coord a, Coord b)
{
    return a.x ==
 b.x && a.y == b.y;
}

const char *env_as_cstr(Env env)
{
    switch (env){
    case ENV_NOTHING: return "ENV_NOTHING";
    case ENV_AGENT:   return "ENV_AGENT";
    case ENV_FOOD:    return "ENV_FOOD";
    case ENV_WALL:    return "ENV_WALL";
    case ENV_COUNT:   return "ENV_COUNT";
    }
    return "Error: env_as_cstr";
}

const char *action_as_cstr(Action action)
{
    switch (action){
    case ACTION_NOP:        return "ACTION_NOP";
    case ACTION_STEP:       return "ACTION_STEP";
    case ACTION_EAT:        return "ACTION_EAT";
    case ACTION_ATTACK:     return "ACTION_ATTACK";
    case ACTION_TURN_LEFT:  return "ACTION_TURN_LEFT";
    case ACTION_TURN_RIGHT: return "ACTION_TURN_RIGHT";
    case ACTION_COUNT:      return "ACTION_COUNT";
    }
    return "Error: action_as_cstr";
}

void print_chromo(FILE *stream, const Chromo *chromo)
{
    for (size_t i = 0; i < GENES_COUNT; ++i) {
        Gene gene = chromo->genes[i];
        fprintf(stream, "STATE=%d ENV=%-13s ACTION=%-18s NEXT_STATE=%d\n",
                gene.state,
                env_as_cstr(gene.env),
                action_as_cstr(gene.action),
                gene.next_state);
    }
}

int is_cell_occupy(const Game *game, Coord pos)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) 
        if (coord_equals(game->agents[i].pos, pos))
            return 1;

    for (size_t i = 0; i < FOODS_COUNT; ++i)
        if (coord_equals(game->foods[i].pos, pos))
            return 1;

    for (size_t i = 0; i < WALLS_COUNT; ++i)
        if (coord_equals(game->walls[i].pos, pos))
            return 1;

    return 0;
}

void init_game(Game *game)
{
    *game = (Game) {0};

    Coord pos = {BOARD_WIDTH, BOARD_HEIGHT};
    for (size_t i = 0; i < AGENTS_COUNT; ++i)
        game->agents[i].pos = pos;
    for (size_t i = 0; i < FOODS_COUNT; ++i)
        game->foods[i].pos = pos;
    for (size_t i = 0; i < WALLS_COUNT ; ++i)
        game->walls[i].pos = pos;
            
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].pos    = random_empty_coord_on_board(game);
        game->agents[i].dir    = random_dir();
        game->agents[i].hunger = HUNGER_MAX;
        game->agents[i].health = HEALTH_MAX;

        for (size_t j = 0; j < GENES_COUNT; ++j) {
            game->chromos[i].genes[j].state = random_int_range(0, STATE_COUNT);
            game->chromos[i].genes[j].env = random_env();
            game->chromos[i].genes[j].action = random_action();
            game->chromos[i].genes[j].next_state = random_int_range(0, STATE_COUNT);
        }
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        game->foods[i].pos = random_empty_coord_on_board(game);
    }

    for (size_t i = 0; i < WALLS_COUNT ; ++i) {
        game->walls[i].pos = random_empty_coord_on_board(game);
    }
}

Coord coord_infront_of_agent(const Agent *agent)
{
    Coord d = coord_dirs[agent->dir];
    Coord result = agent->pos;
    result.x = (result.x + d.x) % BOARD_WIDTH;
    result.y = (result.y + d.y) % BOARD_HEIGHT;
    return result;
}

Food *food_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);
    for (size_t i = 0; i < FOODS_COUNT; i++) {
        if (!game->foods[i].eaten && coord_equals(infront, game->foods[i].pos)) {
            return &game->foods[i];
        }
    }
    return NULL;
}

Agent *agent_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);
    for (size_t i = 0; i < AGENTS_COUNT; i++) {
        if (i != agent_index &&game->agents[i].health > 0 && coord_equals(infront, game->agents[i].pos)) {
            return &game->agents[i];
        }
    }

    return NULL;
}

Wall *wall_infront_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);
    for (size_t i = 0; i < WALLS_COUNT; i++) {
        if (coord_equals(infront, game->walls[i].pos)) {
            return &game->walls[i];
        }
    }

    return NULL;
}

Env env_of_agent(Game *game, size_t agent_index)
{
    if (food_infront_of_agent(game, agent_index) != NULL) {
        return ENV_FOOD;
    }

    if (wall_infront_of_agent(game, agent_index) != NULL) {
        return ENV_WALL;
    }

    if (agent_infront_of_agent(game, agent_index) != NULL) {
        return ENV_AGENT;
    }

    return ENV_NOTHING;
}

void step_agent(Agent *agent)
{
    Coord d = coord_dirs[agent->dir];
    agent->pos.x = (agent->pos.x + d.x) % BOARD_WIDTH;
    agent->pos.y = (agent->pos.y + d.y) % BOARD_HEIGHT;
}

void execute_action(Game *game, size_t agent_index, Action action)
{
    switch (action) {
    
    case ACTION_NOP: {
        
    } break;
    
    case ACTION_STEP:{
        if (env_of_agent(game, agent_index) != ENV_WALL) {
            step_agent(&game->agents[agent_index]);
        }
    }   break;

    case ACTION_EAT: {
        Food *food = food_infront_of_agent(game, agent_index);
        if (food != NULL) {
            food->eaten = 1;
            game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
            if (game->agents[agent_index].hunger > HUNGER_MAX) {
                game->agents[agent_index].hunger = HUNGER_MAX;
            }
        }
    } break;

    case ACTION_ATTACK: {
        // TODO: make agent drop the food when they die
        Agent *other_agent = agent_infront_of_agent(game, agent_index);
        if (other_agent != NULL) {
            other_agent->health -= ATTACK_DAMAGE;
        }
    } break;

    case ACTION_TURN_LEFT: {
        game->agents[agent_index].dir = (game->agents[agent_index].dir + 1) % 4;
    } break;

    case ACTION_TURN_RIGHT: {
        game->agents[agent_index].dir = (game->agents[agent_index].dir + 1) % 4;
    } break;

    case ACTION_COUNT: {
        assert(0 && "Unreachable");
    } break;
    }
}

void step_game(Game *game)
{
    // Interpret genes
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        for (size_t j = 0; j < GENES_COUNT; ++j) {
            Gene gene = game->chromos[i].genes[j];
            if (gene.state == game->agents[i].state &&
                gene.env == env_of_agent(game, i)) {
                execute_action(game, i, gene.action);
                game->agents[i].state = gene.next_state;
            }
        }
    }

    // Handle hunger
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        game->agents[i].hunger -= STEP_HUNGER_DAMAGE;
        if (game->agents[i].hunger <= 0) {
            game->agents[i].health = 0;
        }
    }
}