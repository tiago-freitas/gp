int coord_equals(Coord a, Coord b)
{
    return a.x == b.x && a.y == b.y;
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
    printf("%d\n", (int) env);
    return "Error: env_as_cstr";
}

const char *action_as_cstr(Action action)
{
    switch (action){
    case ACTION_NOP:        return "ACTION_NOP";
    case ACTION_STEP:       return "ACTION_STEP";
    case ACTION_TURN_LEFT:  return "ACTION_TURN_LEFT";
    case ACTION_TURN_RIGHT: return "ACTION_TURN_RIGHT";
    case ACTION_COUNT:      return "ACTION_COUNT";
    }
    return "Error: action_as_cstr";
}

const char *dir_as_cstr(Dir dir)
{
    switch (dir) {
    case DIR_RIGHT: return "DIR_RIGHT";
    case DIR_UP:    return "DIR_UP";
    case DIR_LEFT:  return "DIR_LEFT";
    case DIR_DOWN:  return "DIR_DOWN";
    case DIR_COUNT: return "DIR_COUNT";
    } 
    return "Error: dir_as_cstr";
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

void init_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        Coord pos                = random_empty_coord_on_board(game);
        game->agents[i].pos      = pos;
        game->agents[i].dir      = random_dir();
        game->agents[i].hunger   = HUNGER_MAX;
        game->agents[i].health   = HEALTH_MAX;
        game->agents[i].lifetime = 0;
        game->gameboard[pos.x][pos.y].type = ENV_AGENT;
        game->gameboard[pos.x][pos.y].index = i;

        for (size_t j = 0; j < GENES_COUNT; ++j) {
            game->agents[i].chromo.genes[j].state = random_int_range(0, STATE_COUNT);
            game->agents[i].chromo.genes[j].env = random_env();
            game->agents[i].chromo.genes[j].action = random_action();
            game->agents[i].chromo.genes[j].next_state = random_int_range(0, STATE_COUNT);
        }
    }

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        Coord pos = random_empty_coord_on_board(game);
        game->foods[i].pos = pos;
        game->gameboard[pos.x][pos.y].type  = ENV_FOOD;
        game->gameboard[pos.x][pos.y].index = i;
    }

    for (size_t i = 0; i < WALLS_COUNT ; ++i) {
        Coord pos = random_empty_coord_on_board(game);
        game->walls[i].pos = pos;
        game->gameboard[pos.x][pos.y].type  = ENV_WALL;
        game->gameboard[pos.x][pos.y].index = i;
    }
}

void dump_game(const char *filepath, const Game *game)
{
    FILE *stream = fopen(filepath, "wb");
    if (stream == NULL) {
        fprintf(stderr, "Could not open file: `%s`\n", filepath);
    }
    fwrite(game, sizeof(*game), 1, stream);
    if (ferror(stream)) {
        fprintf(stderr, "Could not dump to file `%s`\n", filepath);
        fclose(stream);
    }
    fclose(stream);

    printf("Dumped current state to `%s`\n", filepath);
}

void load_game(const char *filepath, Game *game)
{
    FILE *stream = fopen(filepath, "rb");
    if (stream == NULL) {
        fprintf(stderr, "Could not open file `%s`\n", filepath);
    }

    size_t n = fread(game, sizeof(*game), 1, stream);
    if (ferror(stream)) {
        fprintf(stderr, "Could not load from file `%s`\n", filepath);
        fclose(stream);
    }

    assert(n == 1);
    printf("Load current state of file `%s`\n", filepath);
    fclose(stream);
}

Agent *agent_at(Game *game, Coord pos)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i)
    {
        if (coord_equals(game->agents[i].pos, pos))
            return &game->agents[i];
    }
    return NULL;
}

void print_agent(FILE *stream, const Agent *agent)
{
    fprintf(stream, "Agent {\n");
    fprintf(stream, " .pos = {%d %d}\n", agent->pos.x, agent->pos.y);
    fprintf(stream, " .dir = %-9s\n", dir_as_cstr(agent->dir));
    fprintf(stream, " .hunger = %d\n", agent->hunger);
    fprintf(stream, " .health = %d\n", agent->health);
    fprintf(stream, " .state = %d\n", agent->state);
    fprintf(stream, " .lifetime = %d\n", agent->lifetime);
    fprintf(stream, " .chromo =\n");
    print_chromo(stream, &agent->chromo);
    fprintf(stream, "}\n");

}

Coord coord_infront_of_agent(const Agent *agent)
{
    Coord d = coord_dirs[agent->dir];
    Coord result = agent->pos;
    result.x = (result.x + d.x + BOARD_WIDTH) % BOARD_WIDTH;
    result.y = (result.y + d.y + BOARD_HEIGHT) % BOARD_HEIGHT;
    return result;
}

Ent env_of_agent(Game *game, size_t agent_index)
{
    Coord infront = coord_infront_of_agent(&game->agents[agent_index]);
    return game->gameboard[infront.x][infront.y];
}

void step_agent(Game *game, Agent *agent)
{
    Coord d = coord_dirs[agent->dir];

    game->gameboard[agent->pos.x][agent->pos.y].type = ENV_NOTHING;
    int i = game->gameboard[agent->pos.x][agent->pos.y].index;
    game->gameboard[agent->pos.x][agent->pos.y].index = 0;

    agent->pos.x = (agent->pos.x + d.x + BOARD_WIDTH) % BOARD_WIDTH;
    agent->pos.y = (agent->pos.y + d.y + BOARD_HEIGHT) % BOARD_HEIGHT;

    game->gameboard[agent->pos.x][agent->pos.y].type = ENV_AGENT;
    game->gameboard[agent->pos.x][agent->pos.y].index = i;

}

void execute_action(Game *game, size_t agent_index, Action action)
{
    switch (action) {


    case ACTION_NOP: {
        
    } break;    

    case ACTION_STEP:{
        Ent ent = env_of_agent(game, agent_index);

        switch (ent.type) {

        case ENV_FOOD: {
            game->foods[ent.index].eaten = 1;
            game->agents[agent_index].hunger += FOOD_HUNGER_RECOVERY;
            if (game->agents[agent_index].hunger > HUNGER_MAX) {
                game->agents[agent_index].hunger = HUNGER_MAX;
            }
            step_agent(game, &game->agents[agent_index]);
        } break;

        case ENV_AGENT: {
            game->agents[ent.index].health -= ATTACK_DAMAGE;
        } break;

        case ENV_NOTHING: {
            step_agent(game, &game->agents[agent_index]);
        } break;

        case ENV_WALL: {

        } break;

        case ENV_COUNT: {
            assert(0 && "Unreachable");
        } break;

        }

    } break;

    case ACTION_TURN_LEFT: {
        game->agents[agent_index].dir = (game->agents[agent_index].dir + 1 + 4) % 4;
    } break;

    case ACTION_TURN_RIGHT: {
        game->agents[agent_index].dir = (game->agents[agent_index].dir - 1 + 4) % 4;
    } break;

    case ACTION_COUNT: {
        assert(0 && "Unreachable");
    } break;

    }

    
}

void step_game(Game *game)
{
    for (size_t i = 0; i < AGENTS_COUNT; ++i) {
        if (game->agents[i].health > 0) {
            // Interpret genes
            for (size_t j = 0; j < GENES_COUNT; ++j) {
                Gene gene = game->agents[i].chromo.genes[j];
                Ent ent = env_of_agent(game, i);
                Env env_type = ent.type;
                if (gene.state == game->agents[i].state && gene.env == env_type) {
                    execute_action(game, i, gene.action);
                    game->agents[i].state = gene.next_state;
                    break;
                }
            }

            // Handle hunger
            game->agents[i].hunger -= STEP_HUNGER_DAMAGE;
            if (game->agents[i].hunger <= 0)
                game->agents[i].health = 0;
            
            if (game->agents[i].health > 0)
                game->agents[i].lifetime++;
        }
    }
}

int compare_agents_lifetimes(const void *a, const void *b) {
    return ( ((const Agent*)b)->lifetime - ((const Agent*)a)->lifetime );
}

void print_best_agents(FILE *stream, Game *game, size_t n)
{
    qsort(&game->agents, AGENTS_COUNT, sizeof(Agent),
          compare_agents_lifetimes);

    if (n > AGENTS_COUNT) {
        n = AGENTS_COUNT;
    }

    for (size_t i = 0; i < n; ++i)
    {
        print_agent(stream, &game->agents[i]);
    }

}

void mate_agents(const Agent *parent1, const Agent *parent2, Agent *child)
{
    const size_t length = (size_t) (GENES_COUNT / 2) * sizeof(Gene);

    memcpy(child->chromo.genes,
           parent1->chromo.genes,
           length);

    // memcpy(child->chromo.genes + (length - 2*sizeof(Gene)),
    //        parent2->chromo.genes + (length - 2*sizeof(Gene)),
    //        length);
}

void mutate_agent(Agent *agent)
{
    for (size_t i = 0; i < GENES_COUNT; ++i) {
        if (random_int_range(0, 100) <= (MUTATION_PROB-1)) {
            agent->chromo.genes[i].state = random_int_range(0, STATE_COUNT);
            agent->chromo.genes[i].env = random_env();
            agent->chromo.genes[i].action = random_action();
            agent->chromo.genes[i].next_state = random_int_range(0, STATE_COUNT);
        }
    }
}

void make_new_generation(Game *prev_game, Game *next_game)
{
    // print_best_agents(stdout, prev_game, SELECTION_POOL);
    qsort(&prev_game->agents, AGENTS_COUNT, sizeof(Agent),
          compare_agents_lifetimes);

    // memset(next_game, 0, sizeof(Game));

    *next_game = (Game) {0};

    for (size_t i = 0; i < FOODS_COUNT; ++i) {
        Coord pos = prev_game->foods[i].pos;
        next_game->foods[i].pos = pos;
        next_game->gameboard[pos.x][pos.y].type = ENV_FOOD;
        next_game->gameboard[pos.x][pos.y].index = i;
    }

    for (size_t i = 0; i < WALLS_COUNT ; ++i) {
        Coord pos = prev_game->foods[i].pos;
        next_game->walls[i].pos = pos;

        next_game->gameboard[pos.x][pos.y].type = ENV_WALL;
        next_game->gameboard[pos.x][pos.y].index = i;

    }

    for (size_t i = 0; i < AGENTS_COUNT; ++i)
    {
        // for (int i = 0; i < BOARD_WIDTH; ++i) {
        //     for (int j = 0; j < BOARD_HEIGHT; ++j) {
        //     printf("%d ", next_game->gameboard[i][j].type);
        //     }
        // }
        // printf("\n");

        size_t p1 = random_int_range(0, SELECTION_POOL);
        size_t p2 = random_int_range(0, SELECTION_POOL);

        mate_agents(&prev_game->agents[p1],
                    &prev_game->agents[p2],
                    &next_game->agents[i]);

        mutate_agent(&next_game->agents[i]);
        Coord pos                     = random_empty_coord_on_board(next_game);
        next_game->agents[i].pos      = pos;
        next_game->agents[i].dir      = random_dir();
        next_game->agents[i].hunger   = HUNGER_MAX;
        next_game->agents[i].health   = HEALTH_MAX;
        next_game->agents[i].lifetime = 0;

        next_game->gameboard[pos.x][pos.y].type = ENV_AGENT;
        next_game->gameboard[pos.x][pos.y].index = i;
    }    
}

int is_everyone_dead(const Game *game)
{
    for (int i = 0; i < AGENTS_COUNT; ++i)
        if (game->agents[i].health > 0)
            return 0;
    return 1;
}
