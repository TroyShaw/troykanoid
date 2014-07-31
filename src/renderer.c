#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>

#include "main.h"
#include "pong.h"
#include "renderer.h"
#include "highscore.h"
#include "powerups.h"

#include "ui/window.h"

//Returns y which considers coordinate system
static float get_y(float y);
static int str_width(char* str);

void renderPostGame();

//fills a rectangle with the given dimensions in the last color that was set
void fillRect(float x, float y, float w, float h);
//draws the outline of a rectangle with the given dimensions in the last color that was set
void drawRect(float x, float y, float w, float h);
//draws a single solid line in the last color that was set
void drawLine(float x1, float y1, float x2, float y2);
//draws a circle in the last color that was set
void drawCircle(float cx, float cy, float r);
//fills a circle in the last color that was set
void fillCircle(float cx, float cy, float r);

//draws text centered on x axis at y value
void centerPrint(float y, char* text, float r, float g, float b, float a);

void set_color3f(int r, int g, int b);
void set_color4f(int r, int g, int b, int a);

static int col_r, col_g, col_b, col_a;
static TTF_Font *font;

void init_renderer(void)
{
    //first init the font library
    if (TTF_Init() != 0)
    {
        printf("failed to init SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    //then get our font
    font = TTF_OpenFont("FreeSans.ttf", 24);

    if (font == NULL)
    {
        printf("failed to open font: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
}

void render(struct Game *game)
{
    clear_screen(0, 0, 0, 0);

    switch (game->mode)
    {
    case MAIN_MENU:
        renderMenu(game);
        break;
    case GAME:
        renderGame(game);
        break;
    case POST_GAME:
        renderPostGame(game);
        break;
    }
}

void renderGame(struct Game *game)
{
    struct PowerupManager manager = game->powerupManager;
    Player player = game->player;
    
    int i, j;

    set_color3f(255, 0, 0);

//draw ball
    for (i = 0; i < game->numBalls; i++)
    {
        Ball ball = game->balls[i];

        if (manager.meteor) set_color3f(255, 25, 25);
        else set_color3f(ball.color.r, ball.color.g, ball.color.b);

        fillCircle(ball.x + ball.radius, ball.y + ball.radius, ball.radius);
    }

//draw powerups
    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        Powerup p = manager.powerups[i];
        if (!p.inUse) continue;

        setPowerupColor(p.type);
        fillRect(p.x, p.y, p.width, p.height);
    }
//end draw powerups

//draw blocks
    for (i = 0; i < BLOCKS_ACROSS; i++)
    {
        for (j = 0; j < BLOCKS_DOWN; j++)
        {

            struct Block* block = &game->level.blocks[i][j];
            if (!block->inUse) continue;

            set_color3f(127, 127, 127);
            drawRect(block->x, block->y, block->width, block->height);
            set_color3f(block->color.r, block->color.g, block->color.b);
            fillRect(block->x, block->y, block->width, block->height);
        }
    }

//draw forcefield
    if (manager.forceField)
    {
        set_color3f(player.color.r, player.color.g, player.color.b);
        fillRect(0, 3, WIDTH, 7);
    }
//end draw forcefield

//draw player
    set_color3f(player.color.r, player.color.g, player.color.b);
    fillRect(player.x, player.y, player.width, player.height);
//end draw player

//info strings
    char lives[25];
    char score[100];
    sprintf(lives, "%s %d", "lives: ", player.lives);
    sprintf(score, "%s %d", "score: ", player.score);
    
    draw_string(0, HEIGHT - 24, lives, 1.0f, 1.0f, 1.0f, 1.0f);
    draw_string(0, HEIGHT - 48, score, 1.0f, 1.0f, 1.0f, 1.0f);
//end draw info strings

//if we are paused, we draw a slightly transparent overly across the whole screen, then the "paused" string
    if (game->paused)
    {
        //TODO
        //glPushMatrix();
        set_color4f(0, 0, 0, 190);

        fillRect(0, 0, WIDTH, HEIGHT);

        char pause[] = "paused";
        char unpause[] = "press P to upause";
        char quit[] = "press Q to quit";

        int offset = 30;

        centerPrint(HEIGHT / 2 + offset, pause, 1.0f, 1.0f, 1.0f, 1.0f);
        centerPrint(HEIGHT / 2 - offset, unpause, 1.0f, 1.0f, 1.0f, 1.0f);
        centerPrint(HEIGHT / 2 - offset * 2, quit, 1.0f, 1.0f, 1.0f, 1.0f);
    }
//end draw pause information

    //glutSwapBuffers();
}

void renderMenu(struct Game *game)
{
    //coordinates of main table
    int x = 75;
    int y = 100;
    //dimensions of main table
    int h = 300;
    int w = WIDTH - x * 2;

    //x coordinate of line divider for the name columns
    int nameX = x + 25;
    //x coordinate of line divider for the score columns
    int scoreX = WIDTH / 2 + 100;
    //height of each row
    int rowHeight = 30;

    //offset for letters from column lines
    int o = 7;

    //string x var (strength length) and temp var for transient use
    int sx, i;

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    set_color3f(255, 0, 0);

//info strings
    char title[] = "Troykanoid!";
    char keys[] = "wasd to move, spacebar to fire ball";
    char start[] = "Push spacebar to start!";

    centerPrint(HEIGHT - 50, title, 1, 1, 1, 1);
    centerPrint(70, keys, 1, 1, 1, 1);
    centerPrint(30, start, 1, 1, 1, 1);
//end info strings

//highscore table
    set_color4f(255, 255, 255, 255);

//main table
//outline
    drawRect(x, y, w, h);
//end outline

//horozontal lines
    for (i = 0; i < 10; i++)
    {
        drawLine(x, y + i * rowHeight, x + w, y + i * rowHeight);
    }
//end horozontal lines

//two vertical lines
    //vertical line after numbers
    drawLine(nameX, y, nameX, y + 9 * rowHeight);

    //vertical line after name
    drawLine(scoreX, y, scoreX, y + 9 * rowHeight);
//end vertical lines
//end table

//vertical numbers (1-9), names, scores
    char header[] = "Highscores";

    centerPrint(y + 9 * rowHeight + o, header, 1, 1, 1, 1);

//numbers from 1-9
    char str[2];
    for (i = 0; i < 9; i++)
    {
        str[0] = (char) (i + '1');
        str[1] = '\0';
        draw_string(x + o, y + (8 - i) * rowHeight + o, str, 1, 1, 1, 1);
    }
//end numbers

//name and score
    //array for score string (15 is a safe amount for our purposes)
    char score[15];
    for (i = 0; i < MAX_SCORES; i++)
    {
        draw_string(nameX + o, y + (8 - i) * rowHeight + o, game->highscoreManager.names[i], 1, 1, 1, 1);

        sprintf(score, "               ");
        sprintf(score, "%d", game->highscoreManager.scores[i]);
        
        sx = x + w - str_width(score) - 5;
        draw_string(sx, y + (8 - i) * rowHeight + o, score, 1, 1, 1, 1);
    }
//end name and score
//end numbers, names and scores
}

void renderPostGame(struct Game *game)
{
    set_color3f(255, 0, 0);

    char won[] = "Congratulations, you completed Troykanoid!";
    char lost[] = "Oh no! You died!";

    char noHS[] = "You didn't make it onto the highscores... Press enter to continue";
    char highscore[] = "You came xxx! Enter your name and press enter!";

    int i = game->highscoreManager.position;

    if (game->currentLevel == NUM_LEVELS)
    {
        centerPrint(HEIGHT - 50, won, 1, 1, 1, 1);
    }
    else
    {
        centerPrint(HEIGHT - 50, lost, 1, 1, 1, 1);
    }

    if (i != -1)
    {
        //add 1st/2nd/3rd etc to string
        //1st - 2nd - 3rd - 4/5/6/7/8/9th
        switch (i)
        {
        case 1:
            highscore[9] = '1';
            highscore[10] = 's';
            highscore[11] = 't';
            break;
        case 2:
            highscore[9]  = '2';
            highscore[10] = 'n';
            highscore[11] = 'd';
            break;
        case 3:
            highscore[9]  = '3';
            highscore[10] = 'r';
            highscore[11] = 'd';
            break;
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            highscore[9]  = '0' + i;
            highscore[10] = 't';
            highscore[11] ='h';
            break;
        }

        centerPrint(HEIGHT - 250, highscore, 1, 1, 1, 1);

        char *nb = game->highscoreManager.nameBuffer;

        centerPrint(HEIGHT - 280, nb, 1, 1, 1, 1);

        //print a flashing curser
        struct timeval tv;
        gettimeofday(&tv, NULL);

        unsigned long long milli = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
        unsigned long long dif = milli - getLastPress();
        if (dif < CURSER_BLINK_RATE || (dif / CURSER_BLINK_RATE) % 2 == 0)
        {
            set_color4f(255, 255, 255, 255);
            //drawRect(x + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)nb), HEIGHT - 283, 1, 23);
        }
    }
    else
    {
        centerPrint(HEIGHT - 250, noHS, 1, 1, 1, 1);
    }

    //glutSwapBuffers();
}

void set_color3f(int r, int g, int b)
{
    col_r = r;
    col_g = g;
    col_b = b;
}

void set_color4f(int r, int g, int b, int a)
{
    col_r = r;
    col_g = g;
    col_b = b;
    col_a = a;
}

void fillRect(float x, float y, float w, float h)
{
    boxRGBA(get_screen(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void drawRect(float x, float y, float w, float h)
{
    rectangleRGBA(get_screen(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void drawLine(float x1, float y1, float x2, float y2)
{
    lineRGBA(get_screen(), x1, get_y(y1), x2, get_y(y2), col_r, col_g, col_b, col_a);
}

void drawCircle(float cx, float cy, float r)
{
    circleRGBA(get_screen(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void fillCircle(float cx, float cy, float r)
{
    filledCircleRGBA(get_screen(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void centerPrint(float y, char* text, float r, float g, float b, float a)
{
    int size_w;
    int size_h;

    TTF_SizeText(font, text, &size_w, &size_h);

    float x = (WIDTH - size_w) / 2;
    draw_string(x, y, text, r, g, b, a);
}

void draw_string(float x, float y, char* text, float r, float g, float b, float a)
{
    SDL_Surface *text_surface;
    SDL_Color text_color = {r * 255, g * 255, b * 255, a * 255};
    text_surface = TTF_RenderText_Blended(font, text, text_color);

    //TODO: the - 30 is a hack because changing to SDL caused a weird offset.
    apply_surface(x, get_y(y) - 27, text_surface);
}

static float get_y(float y)
{
    return HEIGHT - y;
}

static int str_width(char* str)
{
    int size_w;
    int size_h;

    TTF_SizeText(font, str, &size_w, &size_h);

    return size_w;
}
