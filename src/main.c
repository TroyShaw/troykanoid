#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>

#include "defines.h"
#include "fps.h"
#include "game.h"
#include "highscore.h"
#include "imageloader.h"
#include "input.h"
#include "levels.h"
#include "main.h"
#include "renderer.h"
#include "ui/graphics.h"
#include "ui/window.h"

static void resource_init(void);
static void startup_init(void);
static void main_loop(void);
static void clean_up(void);

static void internal_tick(void);
static void internal_render(void);

static void process_events(void);
static void internal_keydown(unsigned char key);

static enum Mode mode;
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
    init_graphics();

    load_images();
    init_highscore_manager(&hsManager);
}

static void startup_init(void)
{
    mode = MAIN_MENU;
    gameRunning = true;

    //initialize the random number generator with new value
    srand(time(NULL));

    fps_init(60);
    init_levels();
    init_game(&game);
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
    cleanup_graphics();
    dispose_images();
}

static void internal_tick(void)
{
    switch (mode)
    {
    case MAIN_MENU:
        if (space_pressed())
        {
            mode = GAME;
            reset_game(&game);
        }
        
        break;
    case GAME:

        //handle pausing
        if (pause_pressed())
        {
            game.paused = !game.paused;
        }

        if (game.paused) return;

        //if game isn't paused, do a tick
        tick_game(&game);

        //check if they've lost or beaten the game
        if (is_game_over(&game) || has_beaten_game(&game))
        {
            set_score(&hsManager, game.player.score);
            mode = POST_GAME;
        }

        break;
    case POST_GAME:

        //nothing to do here, is handled in key-press function
        break;
    }
}

static void internal_render(void)
{
    clear_screen(0, 50, 50, 0);

    switch (mode)
    {
    case MAIN_MENU: renderMenu(&hsManager); break;
    case GAME:      renderGame(&game); break;
    case POST_GAME: renderPostGame(&game, &hsManager); break;
    }

    flip_screen();
}

static void internal_keydown(unsigned char key)
{
    if (key == 27) exit(0);

    if (key == SDLK_k) game.paddle.width += 9;
    if (key == SDLK_l) game.paddle.width -= 9;

    //if it's game-mode, game is paused and they hit 'q', quit the game
    if (mode == GAME && game.paused && key == 'q')
    {
        mode = MAIN_MENU;
    }

    if (mode == POST_GAME)
    {
        switch (key)
        {
        //enter
        case 13:
            if (verify_highscore_name(&hsManager))
            {
                enter_score(&hsManager, game.player.score);
                save_highscores(&hsManager);
                mode = MAIN_MENU;
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
                internal_keydown(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                handle_keyup(event.key.keysym.sym);

                break;
        }
    }

    keyevents_finished();
}
