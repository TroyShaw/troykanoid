#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>

#include "defines.h"
#include "fps.h"
#include "highscore.h"
#include "input.h"
#include "pong.h"
#include "renderer.h"
#include "ui/window.h"

static void resource_init(void);
static void startup_init(void);
static void main_loop(void);
static void clean_up(void);

static void internal_tick(void);
static void internal_render(void);

static void process_events(void);

static struct Game game;
static struct HighscoreManager hsManager;
static bool gameRunning;

int main(void)
{
    resource_init();
    startup_init();

    main_loop();

    clean_up();

    return 0;
}

static void resource_init(void)
{
    init_window(SCREEN_TITLE, WIDTH, HEIGHT);
    init_renderer();

    init_highscore_manager(&hsManager);
}

static void startup_init(void)
{
    gameRunning = true;

    //initialize the random number generator with new value
    srand(time(NULL));
    
    //initialize the fps timer at 60 hz
    fps_init(60);

    initGame(&game);
}

static void main_loop(void)
{
    while (gameRunning && !key_held(SDLK_ESCAPE))
    {
        process_events();

        internal_tick();
        internal_render();

        fps_sleep();
    }
}

static void clean_up(void)
{

}

static void internal_tick(void)
{
    switch (game.mode)
    {
    case MAIN_MENU:
        tickMenu(&game);
        break;
    case GAME:
        tickGame(&game);

        //check if they've lost or beaten the game
        if (isGameOver(&game) || hasBeatenGame(&game))
        {
            set_score(&hsManager, game.player.score);
            game.mode = POST_GAME;
        }

        break;
    case POST_GAME:
        tickPostGame(&game);
        break;
    }
}

static void internal_render(void)
{
    clear_screen(0, 0, 0, 0);

    switch (game.mode)
    {
        case MAIN_MENU: renderMenu(&hsManager); break;
        case GAME:      renderGame(&game); break;
        case POST_GAME: renderPostGame(&game, &hsManager); break;
    }

    flip_screen();
}

static void key(unsigned char key)
{
    if (key == 27) exit(0);

    //if it's game-mode, game is paused and they hit 'q', quit the game
    if (game.mode == GAME && game.paused && key == 'q')
    {
        game.mode = MAIN_MENU;
    }

    if (game.mode == POST_GAME)
    {
        switch (key)
        {
        //enter
        case 13:
            if (verify_highscore_name(&hsManager))
            {
                enter_score(&hsManager, game.player.score);
                save_highscores(&hsManager);
                game.mode = MAIN_MENU;
            }
        default:
            enter_char(&hsManager, key);
        }

        return;
    }

}


static void process_events(void)
{
    static SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                gameRunning = false;

                break;
            case SDL_KEYDOWN:
                handle_keydown(event.key.keysym.sym);
                key(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                handle_keyup(event.key.keysym.sym);

                break;
        }
    }

    keyevents_finished();
}
