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

#define BALL_ARRAY_SIZE 64              //the max number, and size of the array, of balls on screen
#define MAX_BLOCKS 128                  //the max number, and size of the array, of blocks the player can destroy

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 40              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases

//TODO: this isn't strictly true anymore (SDL coordinate system)
//NOTE: coordinate system starts at bottom left (x, y) and goes positive right x, positive up y



//ticks the game. Should be done at around 30 times per second
void tickGame(struct Game *game);

// initializes the game
void initGame(struct Game *game);

//Returns true if the player has run out of lives
bool isGameOver(struct Game *game);

//Returns true if the player has finished the last level and beat the game
bool hasBeatenGame(struct Game *game);
