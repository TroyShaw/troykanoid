#pragma once

#include <stdbool.h>

#include "defines.h"
#include "game_entities.h"
#include "highscore.h"
#include "levels.h"
#include "powerups.h"
#include "util.h"

#define PLAYER_MOVE_SPEED 7             //the movement speed of the player in pixels per tick
#define DEFAULT_LIVES 3                 //the number of lives you get at start of game

#define MAX_BLOCKS 128                  //the max number, and size of the array, of blocks the player can destroy

//TODO: this isn't strictly true anymore (SDL coordinate system)
//NOTE: coordinate system starts at bottom left (x, y) and goes positive right x, positive up y

//Initializes the game and its data structures. Should be called once at program startup.
void init_game(struct Game *game);

//Cleans the game up, freeing any memory, etc. Should be called once at the end of the program.
void cleanup_game(struct Game *game);

//Resets the game back to a starting position. Should be called before starting level 1.
void reset_game(struct Game *game);

//Ticks the game. Should be done at around 30 times per second.
void tick_game(struct Game *game);

//Returns true if the player has run out of lives.
bool is_game_over(struct Game *game);

//Returns true if the player has finished the last level and beat the game.
bool has_beaten_game(struct Game *game);
