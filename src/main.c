#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <SDL/SDL.h>

#include "fps.h"
#include "highscore.h"
#include "main.h"
#include "pong.h"
#include "renderer.h"

#include "input.h"
#include "ui/window.h"

void resource_init(void);
void startup_init(void);
void main_loop(void);
void clean_up(void);

static void internal_tick(void);
static void internal_render(void);

static void process_events(void);

void key(unsigned char key);
void key_up(unsigned char key);

Game game;
static bool gameRunning;

int main(void)
{
    resource_init();
    startup_init();

    main_loop();

    clean_up();

    return 0;
}

void resource_init(void)
{
    init_window(SCREEN_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT);
    loadHighscoresFromDisc();
}

void startup_init(void)
{
    gameRunning = true;

    //initialize the random number generator with new value
    srand(time(NULL));
    
    //initialize the fps timer at 60 hz
    fps_init(60);

    initGame();
}

void main_loop(void)
{
    while (gameRunning && !key_held(SDLK_ESCAPE))
    {
        process_events();

        internal_tick();
        internal_render();

        fps_sleep();
    }
}

void clean_up(void)
{

}

void internal_tick(void)
{
    tick();
}

void internal_render(void)
{
    render();
}

void key(unsigned char key)
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
            if (verifyHighscoreName())
            {
                enterScore(game.player.score);
                saveHighscoresToDisc();
                game.mode = MAIN_MENU;
            }
        default:
            enterChar(key);
        }

        return;
    }

    if (key > 'z') key = key - ('A' - 'a');
    switch (key)
    {
    case 32:
        game.keymanager.space = true;
        break;
    case 'a':
    case 'A':
        game.keymanager.left = true;
        break;
    case 'd':
    case 'D':
        game.keymanager.right = true;
        break;
    case 'p':
    case 'P':
        game.keymanager.pause = true;
    }
}

void key_up(unsigned char key)
{
    if (key > 'z') key = key - ('A' - 'a');
    switch (key)
    {
    case 32:
        game.keymanager.space = false;
        break;
    case 'a':
    case 'A':
        game.keymanager.left = false;
        break;
    case 'd':
    case 'D':
        game.keymanager.right = false;
        break;
    case 'p':
    case 'P':
        game.keymanager.pause = false;
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
                break;
            case SDL_KEYUP:
                handle_keyup(event.key.keysym.sym);

                break;
        }
    }

    keyevents_finished();
}
