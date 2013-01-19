
#include <stdio.h>
#include <stdlib.h>
#include "levels.h"
#include "pong.h"
#include "main.h"

static void initLoad();


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


static const char* LEVEL_FILE = "levels/l3.dat";

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
    int i = 0, x, y, blocks = 0;
    int height = (HEIGHT - 100) / BLOCKS_DOWN;
    int width = WIDTH / BLOCKS_ACROSS;

    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n' || c == 13) continue; //ignore newlines and carriage returns
        y = i / BLOCKS_ACROSS;
        x = i - y * BLOCKS_ACROSS;

        Block* b = &game.blocks[x][y];
        switch(c)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                b->color = colors[c - '0'];
                b->height = height;
                b->width = width;
                b->hitsLeft = 1;
                b->indestructable = false;
                b->inUse = true;
                b->points = points[c - '0'];
                b->type = c - '0';
                b->x = x * width;
                b->y = HEIGHT - (y + 1) * height;
                blocks++;
                break;
            case '9':
                b->color = colors[c - '0'];
                b->height = height;
                b->width = width;
                b->hitsLeft = 1;
                b->indestructable = true;
                b->inUse = true;
                b->points = points[c - '0'];
                b->type = c - '0';
                b->x = x * width;
                b->y = HEIGHT - (y + 1) * height;
                blocks++;
                break;
            case '.':
                b->color = (Color) {0, 0, 0, 0};
                b->height = height;
                b->width = width;
                b->hitsLeft = 1;
                b->indestructable = false;
                b->inUse = false;
                b->points = 0;
                b->type = c - '0';
                b->x = x * width;
                b->y = HEIGHT - (y + 1) * height;
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

    colors[0] = (Color) {1, 1, 1, 1};       // white
    colors[1] = (Color) {1, 0.5, 0, 0};     // orange
    colors[2] = (Color) {0, 0, 0.5, 0};       // light blue
    colors[3] = (Color) {0, 1, 0, 0};       // green
    colors[4] = (Color) {1, 0, 0, 0};   // red
    colors[5] = (Color) {0, 0, 1, 0};   // blue
    colors[6] = (Color) {0.737, 0.43, 0.78, 0};   // pink
    colors[7] = (Color) {1, 1, 0, 0};   // yellow
    colors[8] = (Color) {0.9, 0.91, 0.98, 0};   // silver
    colors[9] = (Color) {0.85, 0.85, 0.1, 0};   // gold

}
