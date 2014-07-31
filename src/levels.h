#pragma once

#include <SDL/SDL.h>

#define NUM_LEVELS 35
#define BLOCKS_ACROSS 11
#define BLOCKS_DOWN 20

struct Block
{
    int x;                                  //the absolute x position of this block
    int y;                                  //the absolute y position of this block
    int width;                              //the width of this block
    int height;                             //the height of this block
    int points;                             //the number of points for destroying this block
    struct SDL_Color color;                 //the color of this block
    int type;                               //the type of this block
    int hitsLeft;                           //the "health" of this block, how many more hits required before it dies
    bool indestructable;                    //if this block is indestructable; balls just bounce off it
    bool inUse;                             //true if we are displaying this
};

struct Level
{
	int blocksLeft;                     		//the amount of breakable blocks left on this level
	struct Block blocks[BLOCKS_ACROSS][BLOCKS_DOWN];	//all the blocks
};

//populates the level array
//if the level is out of bounds, the program aborts
void populateLevel(struct Level *level, int levelNumber);


