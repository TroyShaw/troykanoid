#include <chipmunk/chipmunk.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>

#include "fps.h"
#include "imageloader.h"
#include "powerups.h"
#include "renderer.h"
#include "ui/graphics.h"
#include "ui/window.h"

void renderGame(struct Game *game)
{
    struct PowerupManager manager = game->powerupManager;
    struct Player player = game->player;
    struct Paddle paddle = game->paddle;

    draw_image(0, HEIGHT, background_image());

//draw ball
    for (GSList *l = game->ballList; l != NULL; l = l->next)
    {
        struct Ball *ball = l->data;

        //if (manager.meteor) set_color3f(255, 25, 25);
        //else set_color3f(ball->color.r, ball->color.g, ball->color.b);

        cpVect pos = cpBodyGetPos(ball->ballBody);

        draw_image(pos.x - BALL_RADIUS, pos.y + BALL_RADIUS, ball_image());
    }
//end draw ball

//draw powerups
    for (GSList *l = manager.powerups; l != NULL; l = l->next)
    {
        struct Powerup *p = l->data;
        cpVect pos = cpBodyGetPos(p->powerupBody);

        if (false) //!p->inUse) TODO: make it fade up
        {
            //draw the powerup string as well. Fade upwards for about a second
            int millis = frames_to_millis(frames_game() - p->tick_pickedup);

            if (millis < 1000)
            {
                float dt = millis / 1000.0f;
                draw_string(pos.x, pos.y + 10 + dt * 50, powerup_name(p->type), 1, 1, 1, 1 - 1 * dt);
            }
        }
        else
        {            
            draw_image(pos.x - p->width / 2.0, pos.y + p->height / 2.0, powerup_image());
        }
    }
//end draw powerups

//draw blocks
    for (int i = 0; i < BLOCKS_ACROSS; i++)
    {
        for (int j = 0; j < BLOCKS_DOWN; j++)
        {
            struct Block* block = &game->level.blocks[i][j];
            
            if (!block->inUse) continue;

            cpVect pos = cpBodyGetPos(block->blockBody);

            //TODO why are we adding and subtracting here?
            float blockX = pos.x - block->width / 2.0;
            float blockY = pos.y + block->height / 2.0;

            draw_image(blockX, blockY, block->image);
        }
    }

//draw forcefield
    if (manager.forceField)
    {
        //set_color3f(paddle.color.r, paddle.color.g, paddle.color.b);
        fill_rect(0, 3, WIDTH, 7);
    }
//end draw forcefield

//draw player
    cpVect paddlePos = cpBodyGetPos(game->paddle.paddleBody);
    //coords we display at
    
    float paddleX = paddlePos.x - game->paddle.width / 2.0;
    float paddleY = paddlePos.y - game->paddle.height / 2.0;

    float bumperR = paddle.height / 2.0f;

    draw_image(paddleX - bumperR, paddleY + paddle.height, paddle_left_bumper_image());
    draw_image(paddleX + paddle.width - bumperR, paddleY + paddle.height, paddle_right_bumper_image());

    //to draw the paddle, we draw the first section, the last section, then enough bits in between till the drawn and last overlap
    int paddleImgLength = 64;

    int startXPos = paddleX + bumperR;
    int finalXPos = paddleX + paddle.width - paddleImgLength - bumperR;

    draw_image(startXPos, paddleY + paddle.height, paddle_center_image());
    draw_image(finalXPos, paddleY + paddle.height, paddle_center_image());

    for (int x = startXPos + paddleImgLength; x < finalXPos; x += paddleImgLength)
    {
        draw_image(x, paddleY + paddle.height, paddle_center_image());
    }
//end draw player

//info strings
    set_color4f(150, 150, 150, 100);
    fill_rect(0, HEIGHT - 35, WIDTH, 35);
    
    char scoreString[100];
    sprintf(scoreString, "%s %d", "score: ", player.score);

    draw_string(10, HEIGHT - 25, scoreString, 1.0f, 1.0f, 1.0f, 1.0f);
    
    for (int i = 0; i < player.lives; i++)
    {
        draw_image(WIDTH - i * 20 - 30, HEIGHT - 10, ball_image());
    }
//end draw info strings

//if we are paused, we draw a slightly transparent overly across the whole screen, then the "paused" string
    if (game->paused)
    {
        set_color4f(0, 0, 0, 190);

        fill_rect(0, 0, WIDTH, HEIGHT);

        char pause[] = "paused";
        char unpause[] = "press P to upause";
        char quit[] = "press Q to quit";

        int offset = 30;

        center_print(HEIGHT / 2 + offset, pause, 1.0f, 1.0f, 1.0f, 1.0f);
        center_print(HEIGHT / 2 - offset, unpause, 1.0f, 1.0f, 1.0f, 1.0f);
        center_print(HEIGHT / 2 - offset * 2, quit, 1.0f, 1.0f, 1.0f, 1.0f);
    }
//end draw pause information
}

void renderMenu(struct HighscoreManager *hsManager)
{
    draw_image(0, HEIGHT, background_image());
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

    center_print(HEIGHT - 50, title, 1, 1, 1, 1);
    center_print(70, keys, 1, 1, 1, 1);
    center_print(30, start, 1, 1, 1, 1);
//end info strings

//highscore table
    set_color4f(255, 255, 255, 255);

//main table
//outline
    draw_rect(x, y, w, h);
//end outline

//horozontal lines
    for (i = 0; i < 10; i++)
    {
        draw_line(x, y + i * rowHeight, x + w, y + i * rowHeight);
    }
//end horozontal lines

//two vertical lines
    //vertical line after numbers
    draw_line(nameX, y, nameX, y + 9 * rowHeight);

    //vertical line after name
    draw_line(scoreX, y, scoreX, y + 9 * rowHeight);
//end vertical lines
//end table

//vertical numbers (1-9), names, scores
    char header[] = "Highscores";

    center_print(y + 9 * rowHeight + o, header, 1, 1, 1, 1);

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
        draw_string(nameX + o, y + (8 - i) * rowHeight + o, hsManager->names[i], 1, 1, 1, 1);

        sprintf(score, "               ");
        sprintf(score, "%d", hsManager->scores[i]);
        
        sx = x + w - str_width(score) - 5;
        draw_string(sx, y + (8 - i) * rowHeight + o, score, 1, 1, 1, 1);
    }
//end name and score
//end numbers, names and scores
}

void renderPostGame(struct Game *game, struct HighscoreManager *hsManager)
{
    draw_image(0, HEIGHT, background_image());

    set_color3f(255, 0, 0);

    char won1[] = "Congratulations!";
    char won2[] = "You completed Troykanoid!";
    char lost[] = "Oh no! You died!";

    char noHS1[] = "You didn't make it onto the highscores...";
    char noHS2[] = "Press enter to continue!";

    char prompt[] = "Enter your name and press enter!";

    char highscore[256];
    
    int i = hsManager->position;

    if (game->currentLevel == NUM_LEVELS)
    {
        center_print(HEIGHT - 50, won1, 1, 1, 1, 1);
        center_print(HEIGHT - 80, won2, 1, 1, 1, 1);
    }
    else
    {
        center_print(HEIGHT - 50, lost, 1, 1, 1, 1);
    }

    if (i != -1)
    {
        switch (i)
        {
        case 1:
            sprintf(highscore, "You came %s!", "1st");
            break;
        case 2:
            sprintf(highscore, "You came %s!", "2nd");
            break;
        case 3:
            sprintf(highscore, "You came %s!", "3rd");
            break;
        case 4: case 5: case 6: case 7: case 8: case 9:
            sprintf(highscore, "You came %d%s!", i, "th");
            break;
        }

        center_print(HEIGHT - 180, highscore, 1, 1, 1, 1);
        center_print(HEIGHT - 220, prompt, 1, 1, 1, 1);

        char *nb = hsManager->nameBuffer;

        center_print(HEIGHT - 280, nb, 1, 1, 1, 1);

        //print a flashing curser
        struct timeval tv;
        gettimeofday(&tv, NULL);

        unsigned int dif = get_last_press();

        if (dif < CURSER_BLINK_RATE || (dif / CURSER_BLINK_RATE) % 2 == 0)
        {
            set_color4f(255, 255, 255, 255);
            draw_rect((WIDTH + str_width(nb)) / 2, HEIGHT - 283, 1, 23);
        }
    }
    else
    {
        center_print(HEIGHT - 240, noHS1, 1, 1, 1, 1);
        center_print(HEIGHT - 270, noHS2, 1, 1, 1, 1);
    }
}
