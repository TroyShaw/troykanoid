#include <windows.h>
#include <GL/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
#include "pong.h"
#include "renderer.h"
#include "highscore.h"
#include "powerups.h"

void renderPostGame();

//fills a rectangle with the given dimensions in the last color that was set
void fillRect(float x, float y, float w, float h);
//draws the outline of a rectangle with the given dimensions in the last color that was set
void drawRect(float x, float y, float w, float h);
//draws a rectangle with given dimensions and GL mode
void rect(float x, float y, float w, float h, int mode);
//draws a single solid line in the last color that was set
void drawLine(float x1, float y1, float x2, float y2);
//draws a circle in the last color that was set
void drawCircle(float cx, float cy, float r, int num_segments);
//fills a circle in the last color that was set
void fillCircle(float cx, float cy, float r, int num_segments);

Game game;

void render()
{
    switch (game.mode)
    {
    case GAME:
        renderGame();
        break;
    case MAIN_MENU:
        renderMenu();
        break;
    case POST_GAME:
        renderPostGame();
        break;
    }
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

        //fillRect(ball.x, ball.y, ball.radius * 2, ball.radius * 2);
        fillCircle(ball.x + ball.radius, ball.y + ball.radius, ball.radius, 10);
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

        char* pause = "paused";
        char* unpause = "press P to upause";
        char* quit = "press Q to quit";

        int offset = 30;

        int x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, pause)) / 2;
        glutPrint(x, HEIGHT / 2 + offset, pause, 1.0f, 1.0f, 1.0f, 1.0f);

        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, unpause)) / 2;
        glutPrint(x, HEIGHT / 2 - offset, unpause, 1.0f, 1.0f, 1.0f, 1.0f);

        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, quit)) / 2;
        glutPrint(x, HEIGHT / 2 - offset * 2, quit, 1.0f, 1.0f, 1.0f, 1.0f);
    }
//end draw pause information

    glutSwapBuffers();
}

void renderMenu()
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

//info strings
    char* title = "Troykanoid!";
    sx = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, title)) / 2;
    glutPrint(sx, HEIGHT - 50, title, 1, 1, 1, 1);

    char* keys = "wasd to move, spacebar to fire ball";
    sx = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, keys)) / 2;
    glutPrint(sx, 70, keys, 1, 1, 1, 1);

    char* start = "Push spacebar to start!";
    sx = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, start)) / 2;
    glutPrint(sx, 30, start, 1, 1, 1, 1);
//end info strings

//highscore table
    glColor4f(1, 1, 1, 1);

//main table
//outline
    drawRect(x, y, w, h);
//end outline

//horozontal lines
    for (i = 0; i < 10; i++)
    {
        drawLine(x, y + i * rowHeight, x + w, y + i * rowHeight);
        //glVertex3f(x, y + i * rowHeight, 0);
        //glVertex3f(x + w, y + i * rowHeight, 0);
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
    char* header = "Highscores";
    sx = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, header)) / 2;
    glutPrint(sx, y + 9 * rowHeight + o, header, 1, 1, 1, 1);

//numbers from 1-9
    char str[2];
    for (i = 0; i < 9; i++)
    {
        str[0] = (char) (i + '1');
        str[1] = '\0';
        glutPrint(x + o, y + (8 - i) * rowHeight + o, str, 1, 1, 1, 1);
    }
//end numbers

//name and score
    //array for score string (15 is a safe amount for our purposes)
    char score[15];
    for (i = 0; i < MAX_SCORES; i++)
    {
        glutPrint(nameX + o, y + (8 - i) * rowHeight + o, game.highscoreManager.names[i], 1, 1, 1, 1);

        sprintf(score, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");
        sprintf(score, "%d", game.highscoreManager.scores[i]);
        sx = x + w - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, score) - o;
        glutPrint(sx, y + (8 - i) * rowHeight + o, score, 1, 1, 1, 1);
    }
//end name and score
//end numbers, names and scores

    glutSwapBuffers();
}

void renderPostGame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);

    char *won = "Congratulations, you completed Troykanoid!";
    char *lost = "Oh no! You died!";

    char *noHS = "You didn't make it onto the highscores... Press enter to continue";
    char highscore[] = "You came xxx! Enter your name and press enter!";

    int i = game.highscoreManager.position;
    int x, y;

    if (game.currentLevel == NUM_LEVELS)
    {
        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, won)) / 2;
        glutPrint(x, HEIGHT - 50, won, 1, 1, 1, 1);
    }
    else
    {
        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, lost)) / 2;
        glutPrint(x, HEIGHT - 50, lost, 1, 1, 1, 1);
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

        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, highscore)) / 2;
        glutPrint(x, HEIGHT - 250, highscore, 1, 1, 1, 1);

        char *nb = game.highscoreManager.nameBuffer;
        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, nb)) / 2;
        glutPrint(x, HEIGHT - 280, nb, 1, 1, 1, 1);

        //print a flashing curser
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long milli = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
        unsigned long long dif = milli - getLastPress();
        if (dif < CURSER_BLINK_RATE || (dif / CURSER_BLINK_RATE) % 2 == 0)
        {
            glColor4f(1.0, 1.0, 1.0, 1.0);
            drawRect(x + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, nb), HEIGHT - 283, 1, 23);
        }
    }
    else
    {
        x = (WIDTH - glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, noHS)) / 2;
        glutPrint(x, HEIGHT - 250, noHS, 1, 1, 1, 1);
    }

    glutSwapBuffers();
}

void fillRect(float x, float y, float w, float h)
{
    rect(x, y, w, h, GL_POLYGON);
}

void drawRect(float x, float y, float w, float h)
{
    rect(x, y, w, h, GL_LINE_LOOP);
}

void rect(float x, float y, float w, float h, int mode)
{
    glBegin(mode);
    {
        glVertex3f(x, y, 0);
        glVertex3f(x + w, y, 0);
        glVertex3f(x + w, y + h, 0);
        glVertex3f(x, y + h, 0);
    }
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2)
{
    glBegin(GL_LINES);
    {
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd();
}


//courtesy of http://slabode.exofire.net/circle_draw.shtml
void drawCircle(float cx, float cy, float r, int num_segments)
{
	float theta = 2 * 3.1415926 / (float)(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0
	float y = 0;

	int i;

	glBegin(GL_LINE_LOOP);
	for(i = 0; i < num_segments; i++)
	{
		glVertex2f(x + cx, y + cy);//output vertex

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}
	glEnd();
}

//courtesy of http://slabode.exofire.net/circle_draw.shtml
void fillCircle(float cx, float cy, float r, int num_segments)
{
	float theta = 2 * 3.1415926 / (float)(num_segments);
	float c = cosf(theta);//precalculate the sine and cosine
	float s = sinf(theta);
	float t;

	float x = r;//we start at angle = 0
	float y = 0;

	int i;

	glBegin(GL_POLYGON);
	for(i = 0; i < num_segments; i++)
	{
		glVertex2f(x + cx, y + cy);//output vertex

		//apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
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
