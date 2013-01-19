#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "pong.h"
#include "renderer.h"
#include "highscore.h"

//fills a rectangle with the given dimensions in the last color that was set
void fillRect(int x, int y, int w, int h);
//draws the outline of a rectangle with the given dimensions in the last color that was set
void drawRect(int x, int y, int w, int h);

Game game;

void render()
{
    if (game.running)   renderGame();
    else                renderMenu();
}

void renderGame()
{
    PowerupManager manager = game.powerupManager;
    Player player = game.player;
    //const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);


//draw ball
    for (i = 0; i < game.numBalls; i++)
    {
        Ball ball = game.balls[i];

        if (manager.meteor) glColor3f(1, 0.1, 0.1);
        else glColor3f(ball.color.r, ball.color.g, ball.color.b);
        fillRect(ball.x, ball.y, ball.radius * 2, ball.radius * 2);
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

            Block* block = &game.blocks[i][j];
            if (!block->inUse) continue;

            glColor3f(0.5, 0.5, 0.5);
            drawRect(block->x, block->y, block->width, block->height);
            glColor3f(block->color.r, block->color.g, block->color.b);
            fillRect(block->x, block->y, block->width, block->height);
        }
    }

//draw forcefield
    if (manager.forceField)
    {
        glColor3f(player.color.r, player.color.g, player.color.b);
        fillRect(0, 3, WIDTH, 7);
    }
//end draw forcefield

//draw player
    glColor3f(player.color.r, player.color.g, player.color.b);
    fillRect(player.x, player.y, player.width, player.height);
//end draw player

//info strings
    char lives[25];
    char score[100];
    sprintf(lives, "%s %d", "lives: ", player.lives);
    sprintf(score, "%s %d", "score: ", player.score);
    glutPrint(0, HEIGHT - 24, lives, 1.0f, 1.0f, 1.0f, 1.0f);
    glutPrint(0, HEIGHT - 48, score, 1.0f, 1.0f, 1.0f, 1.0f);
//end draw info strings

//if we are paused, we draw a slightly transparent overly across the whole screen, then the "paused" string
    if (game.paused)
    {
        glPushMatrix();
        glColor4f(0, 0, 0, 0.75);

        fillRect(0, 0, WIDTH, HEIGHT);

        char* pause = "paused - press P to unpause";
        int x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, pause)) / 2;
        glutPrint(x, HEIGHT / 2, pause, 1.0f, 1.0f, 1.0f, 1.0f);
    }
//end draw pause information

    glutSwapBuffers();
}

void renderMenu()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

    char* title = "Troykanoid!";
    int x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, title)) / 2;
    glutPrint(x, HEIGHT - 50, title, 1, 1, 1, 1);

    char* keys = "wasd to move, spacebar to fire ball";
    x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, keys)) / 2;
    glutPrint(x, 70, keys, 1, 1, 1, 1);

    char* start = "Push spacebar to start!";
    x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, start)) / 2;
    glutPrint(x, 30, start, 1, 1, 1, 1);

    //highscore table

    glPushMatrix();
    glColor4f(1, 1, 1, 1);

    x = 100;
    int y = 100;
    int h = 300;
    int w = WIDTH - y * 2;

    int nameX = 30;
    int scoreX = WIDTH / 2;

    drawRect(x, y, w, h);

    glBegin(GL_LINES);
    {
        int i;
        for (i = 0; i < 10; i++)
        {
            glVertex3f(x, y + i * 30, 0);
            glVertex3f(x + w, y + i * 30, 0);
        }
    }
    glEnd();

    glBegin(GL_LINES);
    {
        //vertical line after numbers
        glVertex3f(x + nameX, y, 0);
        glVertex3f(x + nameX, y + 9 * 30, 0);

        //vertical line after name
        glVertex3f(scoreX, y, 0);
        glVertex3f(scoreX, y + 9 * 30, 0);
    }
    glEnd();
    glPopMatrix();

    //now the actual text
    int o = 7;

    char* header = "Highscores";
    x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, header)) / 2;
    glutPrint(x, y + 9 * 30 + o, header, 1, 1, 1, 1);

    int i;
    char* str = malloc(2 * sizeof(char));

    for (i = 0; i < 9; i++)
    {
        str[0] = (char) (i + '1');
        str[1] = '\0';
        glutPrint(108, y + (8 - i) * 30 + o, str, 1, 1, 1, 1);
    }
    free(str);

    char score[15];

    for (i = 0; i < MAX_SCORES; i++)
    {
        glutPrint(100 + nameX, y + (8 - i) * 30 + o, game.highscoreManager.names[i], 1, 1, 1, 1);

        sprintf(score, "              ");
        sprintf(score, "%d", game.highscoreManager.scores[i]);
        glutPrint(WIDTH / 2 + 5, y + (8 - i) * 30 + o, score, 1, 1, 1, 1);
    }

    glutSwapBuffers();
}

void fillRect(int x, int y, int w, int h)
{
    glBegin(GL_POLYGON);
    {
        glVertex3f(x, y, 0);
        glVertex3f(x + w, y, 0);
        glVertex3f(x + w, y + h, 0);
        glVertex3f(x, y + h, 0);
    }
    glEnd();
}

void drawRect(int x, int y, int w, int h)
{
    glBegin(GL_LINE_LOOP);
    {
        glVertex3f(x, y, 0);
        glVertex3f(x + w, y, 0);
        glVertex3f(x + w, y + h, 0);
        glVertex3f(x, y + h, 0);
    }
    glEnd();
}

void glutPrint(float x, float y, char* text, float r, float g, float b, float a)
{
    if(!text || !strlen(text)) return;

    glColor4f(r,g,b,a);
    glRasterPos2f(x,y);
    while (*text)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *text);
        text++;
    }
}
