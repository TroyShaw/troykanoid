#pragma once

#include <stdbool.h>
#include <SDL/SDL.h>
#include <chipmunk/chipmunk.h>

#define NUM_LEVELS 35
#define BLOCKS_ACROSS 11
#define BLOCKS_DOWN 20

#define BLOCK_WIDTH 58
#define BLOCK_HEIGHT 20

//because the blocks don't fit perfectly on-screen, there is a 1 pixel offset to center them
#define BLOCK_OFFSET 1

struct Block
{
    cpBody *blockBody;
    cpShape *blockShape;

    //int x;                                  //the absolute x position of this block
    //int y;                                  //the absolute y position of this block
    int width;                              //the width of this block
    int height;                             //the height of this block
    int points;                             //the number of points for destroying this block
    struct SDL_Color color;                 //the color of this block
    int type;                               //the type of this block
    int hitsLeft;                           //the "health" of this block, how many more hits required before it dies
    bool indestructable;                    //if this block is indestructible; balls just bounce off it
    bool inUse;                             //true if we are displaying this
    bool collided;                          //true if this block was active this tick but has collided with a ball and should be processed
    SDL_Surface *image;                     //the image. TODO: create a proper type and then a get_brick_image(type) method?
};

struct Level
{
    int blocksLeft;                                         //the amount of breakable blocks left on this level
    struct Block blocks[BLOCKS_ACROSS][BLOCKS_DOWN];        //all the blocks
};

//Populates the level array.
//If the level is out of bounds, the program aborts.
void populate_level(struct Level *level, int levelNumber);
void init_levels(void);
