#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "defines.h"
#include "levels.h"

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
struct SDL_Color colors[10];
int points[9];

static char LEVEL_FILE[] = "levels/lxx.dat";

// Game game;

void populateLevel(struct Level *level, int levelNumber)
{
    if (!hasLoaded) initLoad();

    if (levelNumber < 1 || levelNumber > NUM_LEVELS)
    {
        printf("tried to load invalid level %d\n", levelNumber);
        exit(1);
    }

    if (levelNumber < 10)
    {
        LEVEL_FILE[8] = '0';
        LEVEL_FILE[9] = (char) ('0' + levelNumber);
    }
    else
    {
        LEVEL_FILE[8] = (char) ('0' + levelNumber / 10);
        LEVEL_FILE[9] = (char) ('0' + levelNumber % 10);
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

        struct Block* b = &level->blocks[x][y];

        b->width = width;
        b->height = height;
        b->x = x * width;
        b->y = HEIGHT - (y + 1) * height;
        b->type = c - '0';

        switch(c)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                b->color = colors[c - '0'];
                b->hitsLeft = 1;
                b->indestructable = false;
                b->inUse = true;
                b->points = points[c - '0'];
                blocks++;
                break;
            case '9':
                b->color = colors[c - '0'];
                b->indestructable = true;
                b->inUse = true;
                break;
            case '.':
                b->color = (SDL_Color) {0, 0, 0, 0};
                b->inUse = false;
                break;
            default:
                printf("loaded wrong block %c/ %d\nprogram closing\b", c, c);
                exit(1);
        }

        i++;
    }

    level->blocksLeft = blocks;
}

static void initLoad()
{
    hasLoaded = true;
    int i;

    for (i = 0; i < 9; i++) points[i] = 50 + i * 10;

    colors[0] = (SDL_Color) {255, 255, 255, 0};           // white
    colors[1] = (SDL_Color) {255, 127,   0, 0};         // orange
    colors[2] = (SDL_Color) {  0,   0, 127, 0};         // light blue
    colors[3] = (SDL_Color) {  0, 255,   0, 0};           // green
    colors[4] = (SDL_Color) {255,   0,   0, 0};           // red
    colors[5] = (SDL_Color) {  0,   0, 255, 0};           // blue
    colors[6] = (SDL_Color) {188, 110, 199, 0}; // pink
    colors[7] = (SDL_Color) {255, 255,   0, 0};           // yellow
    colors[8] = (SDL_Color) {230, 232, 250, 0};   // silver
    colors[9] = (SDL_Color) {217, 217,  26, 0};   // gold
}
