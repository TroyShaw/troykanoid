#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include "pong.h"
#include "renderer.h"
#include "highscore.h"

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
    int i;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);


//draw ball
    for (i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        Ball ball = game.balls[i];
        if (!ball.inUse) continue;
        glPushMatrix();
        glColor3f(ball.color.r, ball.color.g, ball.color.b);
        glBegin(GL_POLYGON);
        {
            int x = ball.x;
            int y = ball.y;
            int h = ball.radius * 2;
            int w = h;

            glVertex3f(x, y, 0);
            glVertex3f(x + w, y, 0);
            glVertex3f(x + w, y + h, 0);
            glVertex3f(x, y + h, 0);
        }
        glEnd();
        glPopMatrix();
    }



//draw powerups

    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        Powerup p = manager.powerups[i];

        if (!p.inUse) continue;

        glPushMatrix();
        setPowerupColor(p.type);
        glBegin(GL_POLYGON);
        {
            int x = p.x;
            int y = p.y;
            int h = p.height;
            int w = p.width;

            glVertex3f(x, y, 0);
            glVertex3f(x + w, y, 0);
            glVertex3f(x + w, y + h, 0);
            glVertex3f(x, y + h, 0);
        }
        glEnd();
        glPopMatrix();
    }

//end draw powerups

//draw forcefield
    if (manager.forceField)
    {
        glPushMatrix();
        glColor3f(player.color.r, player.color.g, player.color.b);
        glBegin(GL_POLYGON);
        {
            int x = 0;
            int y = 3;
            int h = 7;
            int w = WIDTH;

            glVertex3f(x, y, 0);
            glVertex3f(x + w, y, 0);
            glVertex3f(x + w, y + h, 0);
            glVertex3f(x, y + h, 0);
        }
        glEnd();
        glPopMatrix();
    }
//end draw forcefield

//draw player
    glPushMatrix();
    glColor3f(player.color.r, player.color.g, player.color.b);
    glBegin(GL_POLYGON);
    {
        int x = player.x;
        int y = player.y;
        int h = player.height;
        int w = player.width;

        glVertex3f(x, y, 0);
        glVertex3f(x + w, y, 0);
        glVertex3f(x + w, y + h, 0);
        glVertex3f(x, y + h, 0);
    }
    glEnd();
    glPopMatrix();
//end draw player

    //info strings
    char lives[25];
    char score[100];
    sprintf(lives, "%s %d", "lives: ", player.lives);
    sprintf(score, "%s %d", "score: ", player.score);
    glutPrint(0, HEIGHT - 24, lives, 1.0f, 1.0f, 1.0f, 1.0f);
    glutPrint(0, HEIGHT - 48, score, 1.0f, 1.0f, 1.0f, 1.0f);


    //if we are paused, we draw a slightly transparent overly across the whole screen, then the "paused" string
    if (game.paused)
    {
        glPushMatrix();
        glColor4f(0, 0, 0, 0.75);

        glBegin(GL_POLYGON);
        {
            int x = 0;
            int y = 0;
            int h = HEIGHT;
            int w = WIDTH;

            glVertex3f(x, y, 0);
            glVertex3f(x + w, y, 0);
            glVertex3f(x + w, y + h, 0);
            glVertex3f(x, y + h, 0);
        }
        glEnd();
        glPopMatrix();

        char* pause = "paused - press P to unpause";
        int x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, pause)) / 2;
        glutPrint(x, HEIGHT / 2, pause, 1.0f, 1.0f, 1.0f, 1.0f);
    }


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

    glBegin(GL_LINE_LOOP);
    {
        glVertex3f(x, y, 0);
        glVertex3f(x + w, y, 0);
        glVertex3f(x + w, y + h, 0);
        glVertex3f(x, y + h, 0);
    }
    glEnd();

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
        //printf("%d\n", game.highscoreManager.scores[i]);
        glutPrint(WIDTH / 2 + 5, y + (8 - i) * 30 + o, score, 1, 1, 1, 1);
    }

    glutSwapBuffers();
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
