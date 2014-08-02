#pragma once

#include <stdbool.h>

#include "defines.h"
#include "game_entities.h"
#include "highscore.h"
#include "levels.h"
#include "powerups.h"
#include "util.h"

#define PLAYER_MOVE_SPEED 7             //the movement speed of the player in pixels per tick
#define PADDLE_DEFAULT_WIDTH 100        //the default paddle size, and size it's set to on respawn
#define DEFAULT_LIVES 3                 //the number of lives you get at start of game

#define MAX_BLOCKS 128                  //the max number, and size of the array, of blocks the player can destroy

//TODO: this isn't strictly true anymore (SDL coordinate system)
//NOTE: coordinate system starts at bottom left (x, y) and goes positive right x, positive up y

//Ticks the game. Should be done at around 30 times per second.
void tick_game(struct Game *game);

//Initializes the game.
void init_game(struct Game *game);

//Returns true if the player has run out of lives.
bool is_game_over(struct Game *game);

//Returns true if the player has finished the last level and beat the game.
bool has_beaten_game(struct Game *game);
