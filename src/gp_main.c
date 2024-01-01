#include "./gp_game.c"
#include "./gp_visual.c"

Game game = {0};

int main(void)
{
    // srand(time(0));

    init_game(&game);

    print_chromo(stdout, &game.chromos[0]);

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
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    // scc(SDL_RenderSetLogicalSize(
    //         renderer,
    //         SCREEN_WIDTH,
    //         SCREEN_HEIGHT));

    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");


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
                case SDLK_r: {
                    init_game(&game);
                } break;
                }
            } break;
            }
        }

        scc(SDL_SetRenderDrawColor(renderer, HEX_COLOR(BACKGROUND_COLOR)));
        scc(SDL_RenderClear(renderer));

        render_grid_board(renderer);
        render_game(renderer, &game);

        SDL_RenderPresent(renderer);
        // quit = 1;
    }

    SDL_Quit();
    return 0;
}