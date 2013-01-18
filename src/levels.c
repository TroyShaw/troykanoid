
#include <stdio.h>
#include <stdlib.h>
#include "levels.h"
#include "pong.h"
#include "main.h"

static void initLoad();

//
// 0 = white,           50 points
// 1 = orange,          60 points
// 2 = light blue,      70 points
// 3 = green,           80 points
// 4 = red,             90 points
// 5 = blue,            100 points
// 6 = pink,            110 points
// 7 = yellow,          120 points
// 8 = silver,          50 pts x round number
// 9 = gold,            (indestructable)
// . = empty,           no block

bool hasLoaded = false;
Color colors[10];
int points[9];


static const char* LEVEL_FILE = "levels.dat";

Game game;
void populateLevel(int level)
{
    if (!hasLoaded) initLoad();

    if (level < 1 || level > NUM_LEVELS)
    {
        printf("tried to load invalid level %d\n", level);
        exit(1);
    }

    FILE *fp = fopen(LEVEL_FILE, "r");

    if (fp == NULL)
    {
        printf("level file did not exist\n");
        exit(1);
    }

    printf("loading level file...\n");

    char c;
    int i = 0, x, y;

    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n') continue; //ignore newlines
        x = i % BLOCKS_ACROSS;
        y = i / BLOCKS_DOWN;

        Block* b = &game.blocks[x][y];
        switch(c)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                b->color = (Color) {1, 0.1, 0.1, 1};
                b->height = (HEIGHT - 50) / BLOCKS_DOWN;
                b->width = WIDTH / BLOCKS_ACROSS;
                b->hitsLeft = 1;
                b->indestructable = false;
                b->inUse = true;
                b->points = points[c - '0'];
                b->type = c - '0';
                b->x = x * b->width;
                b->y = HEIGHT - y * b->height;
                break;
            case '9':
                //do gold block stuff here
                break;
            case '.':
                b->inUse = false;
                break;
            default:
                printf("loaded wrong block %c/ %d\nprogram closing\b", c, c);
                exit(1);
                //break;
        }

        i++;
    }

}

static void initLoad()
{
    hasLoaded = true;
    int i;

    for (i = 0; i < 9; i++) points[i] = 50 + i * 10;
    //for ()


}
