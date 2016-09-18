#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

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

static void startup_init(void);
static void main_loop(void);
static void clean_up(void);

static void internal_tick(void);
static void internal_render(void);

static void process_events(void);
static void internal_keydown(SDL_Keysym key);

static enum Mode mode;
static struct Game game;
static struct HighscoreManager hsManager;
static bool gameRunning;

int main(void)
{
    startup_init();

    main_loop();

    clean_up();

    return 0;
}

static void startup_init(void)
{
    init_window(SCREEN_TITLE, WIDTH, HEIGHT);
    init_graphics();
    load_images();

    init_highscore_manager(&hsManager);
    init_levels();
    init_game(&game);

    fps_init(60);
    srand(time(NULL));
    
    mode = MAIN_MENU;
    gameRunning = true;
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
    dispose_images();
    cleanup_graphics();
}

static void internal_tick(void)
{

    if (pause_pressed()) 
    {
        printf("pause pressed\n");
    }
    
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

static void internal_keydown(SDL_Keysym keysym)
{
    unsigned char key = keysym.sym;

    bool should_capatilise = FALSE;

    if (keysym.mod & KMOD_CAPS) should_capatilise = !should_capatilise;
    if (keysym.mod & KMOD_SHIFT) should_capatilise = !should_capatilise;

    unsigned char typed_key = should_capatilise ? toupper(key) : key;

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
            if (made_highscore(&hsManager) && verify_highscore_name(&hsManager))
            {
                enter_score(&hsManager, game.player.score);
                save_highscores(&hsManager);
                mode = MAIN_MENU;
            }

            if (!made_highscore(&hsManager))
            {
                mode = MAIN_MENU;
            }

        default:
            enter_char(&hsManager, typed_key);
        }

        return;
    }

}

static void process_events(void)
{
    static SDL_Event event;

    int length;
    const Uint8 *state = SDL_GetKeyboardState(&length);
    //printf("length of array: %i\n", length);

    while (SDL_PollEvent(&event))
    {
        int key;

        switch (event.type)
        {
            case SDL_QUIT:
                gameRunning = false;

                break;
            case SDL_KEYDOWN:
                key = event.key.keysym.sym;

                printf("text: %d, %d\n", event.key.keysym.sym, SDL_GetKeyFromScancode(event.key.keysym.scancode));
                  printf("Physical %s key acting as %s key, %d\n",
                    SDL_GetScancodeName(event.key.keysym.scancode),
                      SDL_GetKeyName(event.key.keysym.sym),
                      SDL_GetScancodeFromKey(event.key.keysym.sym));


                handle_keydown(SDL_GetScancodeFromKey(event.key.keysym.sym));
                internal_keydown(event.key.keysym);
                break;
            case SDL_KEYUP:
                handle_keyup(SDL_GetScancodeFromKey(event.key.keysym.sym));

                break;
        }
    }

    keyevents_finished();
}

