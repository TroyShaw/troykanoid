#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "defines.h"
#include "game_entities.h"
#include "imageloader.h"
#include "levels.h"

//Returns the correct image for the given brick
static struct Texture get_image(char brickchar);

//colored bricks take 1 hit each
//silver bricks take 2 + level / 8

// 0 = white,           50 points
// 1 = orange,          60 points
// 2 = light blue,      70 points
// 3 = green,           80 points
// 4 = red,             90 points
// 5 = blue,            100 points
// 6 = pink,            110 points
// 7 = yellow,          120 points
// 8 = silver,          50 pts x round number
// 9 = gold,            (indestructible)
// . = empty,           no block

struct SDL_Color colors[10];
static int points[8];

//we make a full level file name with LEVEL_FILE_PREFIX + levelNum + LEVEL_FILE_SUFFIX
static char LEVEL_FILE_PREFIX[] = "levels/l";
static char LEVEL_FILE_SUFFIX[] = ".dat";

void populate_level(struct Level *level, int levelNumber)
{
    if (levelNumber < 1 || levelNumber > NUM_LEVELS)
    {
        printf("tried to load invalid level %d\n", levelNumber);
        exit(1);
    }

    char filename[255];
    snprintf(filename, 255, "%s%d%s", LEVEL_FILE_PREFIX, levelNumber, LEVEL_FILE_SUFFIX);

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("level file did not exist\n");
        exit(1);
    }

    printf("loading level file: %s\n", filename);

    char c;
    int i = 0, x, y, blocks = 0;
    int height = BLOCK_HEIGHT;
    int width = BLOCK_WIDTH;
    printf("width %d, height %d\n", width, height);
    
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\n' || c == 13) continue; //ignore newlines and carriage returns
        
        y = i / BLOCKS_ACROSS;
        x = i - y * BLOCKS_ACROSS;

        struct Block* b = &level->blocks[x][y];

        //subtract 1 from width and height so that the blocks don't overlap at edges 1 pixel
        //b->width = width - 1;
        //b->height = height - 1;
        //b->x = x * width + BLOCK_OFFSET;
        //b->y = HEIGHT - (y + 1) * height;
        //b->type = c - '0';

        switch(c)
        {
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                b->image = get_image(c);
                b->color = colors[c - '0'];
                b->indestructable = false;
                b->inUse = true;
                cpShapeSetLayers(b->blockShape, BALL_BLOCK_LAYER);
                blocks++;
                
                if (c == '8')
                {
                    //silver block
                    b->hitsLeft = 2 + levelNumber / 8;
                    b->points = levelNumber * 50;
                }
                else
                {
                    //colored block
                    b->hitsLeft = 1;
                    b->points = points[c - '0'];
                }

                break;
            case '9':
                b->image = get_image(c);
                b->color = colors[c - '0'];
                b->indestructable = true;
                b->inUse = true;
                cpShapeSetLayers(b->blockShape, BALL_BLOCK_LAYER);
                break;
            case '.':
                b->color = (SDL_Color) {0, 0, 0, 0};
                b->inUse = false;
                cpShapeSetLayers(b->blockShape, UNUSED_BLOCK_LAYER);
                break;
            default:
                printf("loaded wrong block %c/ %d\nprogram closing\b", c, c);
                exit(1);
        }

        i++;
    }

    level->blocksLeft = blocks;

    printf("finished loading level %d\n", levelNumber);
}

void init_levels(void)
{
    for (int i = 0; i < 8; i++)
    {
        points[i] = 50 + i * 10;
    }

    colors[0] = (SDL_Color) {255, 255, 255, 0};     // white
    colors[1] = (SDL_Color) {255, 127,   0, 0};     // orange
    colors[2] = (SDL_Color) {  0,   0, 127, 0};     // light blue
    colors[3] = (SDL_Color) {  0, 255,   0, 0};     // green
    colors[4] = (SDL_Color) {255,   0,   0, 0};     // red
    colors[5] = (SDL_Color) {  0,   0, 255, 0};     // blue
    colors[6] = (SDL_Color) {188, 110, 199, 0};     // pink
    colors[7] = (SDL_Color) {255, 255,   0, 0};     // yellow
    colors[8] = (SDL_Color) {230, 232, 250, 0};     // silver
    colors[9] = (SDL_Color) {217, 217,  26, 0};     // gold
}

static struct Texture get_image(char brickchar)
{
    switch (brickchar)
    {
        case '0': return brick_white_image();
        case '1': return brick_orange_image();
        case '2': return brick_cyan_image();
        case '3': return brick_green_image();
        case '4': return brick_red_image();
        case '5': return brick_blue_image();
        case '6': return brick_pink_image();
        case '7': return brick_yellow_image();
        case '8': return brick_silver_image();
        case '9': return brick_gold_image();
    }

    printf("error getting brick image: %c\n", brickchar);
    exit(1);
}
