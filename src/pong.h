#pragma once

#include <stdbool.h>

#include "defines.h"
#include "highscore.h"
#include "levels.h"
#include "powerups.h"
#include "util.h"

typedef enum
{
    MAIN_MENU,
    POST_GAME,
    GAME
} GameMode;

#define PLAYER_MOVE_SPEED 7             //the movement speed of the player in pixels per tick
#define PADDLE_DEFAULT_WIDTH 100        //the default paddle size, and size it's set to on respawn
#define DEFAULT_LIVES 3                 //the number of lives you get at start of game

#define BALL_ARRAY_SIZE 64              //the max number, and size of the array, of balls on screen
#define MAX_BLOCKS 128                  //the max number, and size of the array, of blocks the player can destroy

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 40              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases
#define PADDLE_CHANGE 40                //the amount of pixels the paddle increases/ decreases by when picking up that powerup

//TODO: this isn't strictly true anymore (SDL coordinate system)
//NOTE: coordinate system starts at bottom left (x, y) and goes positive right x, positive up y

typedef struct
{
    float x;                              //x coord
    float y;                              //y coord
    int radius;                         //radius of ball
    SDL_Color color;                        //its color
    float velX;                         //velocity in x axis
    float velY;                         //velocity in y axis
    bool inUse;                         //true if ball is being displayed/ etc, on screen right now
} Ball;

typedef struct
{
    int score;                          //score of player
    float x;                            //x coord
    float y;                            //y coord
    int width;                          //current width of paddle, drawing/ calculations will be done using this
    int realWidth;                      //actual width. current width will approach this each tick (to make a smooth transition)
    int height;                         //height of paddle
    int lives;                          //number of lives the player has left. Getting 0 will mean game over
    SDL_Color color;                    //color of paddle
} Player;

struct Paddle
{
    float x;
    float y;
    int width;
    int realWidth;
    int height;
    SDL_Color color;
};

typedef struct
{
    bool left;                              //true if left is currently held
    bool right;                             //true if right is currently held
    bool space;                             //true if space is currently held
    bool pause;                             //true if the pause button is currently held
} Keymanager;

struct Game
{
    GameMode mode;                      //the mode of the game
    bool paused;                        //true if we are in a game and paused, false we are not paused
    int currentLevel;                   //the current level the player is on
    struct Level level;                 //the level struct
    Player player;                      //the player
    Ball balls[BALL_ARRAY_SIZE];        //the balls
    int numBalls;                       //number of balls in use (useful for various reasons)
    bool attached;                      //if the ball is attached to paddle (i.e at start of games and after death)
    struct PowerupManager powerupManager;      //the manager for powerups
};


//ticks the game. Should be done at around 30 times per second
void tick(struct Game *game);
void tickMenu(struct Game *game);
void tickGame(struct Game *game);
void tickPostGame();


//moves the player
void movePlayer(struct Game *game);

//moves all the ball, (to be used when ball isn't attached). Keeps them within bounds of left/right/ceiling
void moveBalls(struct Game *game);

//moves the first ball to players location, should be used when ball is attached
void moveBallToPlayer(struct Game *game);

//test collision of balls and blocks
void ballBlockCollisions(struct Game *game);

//test collision of player vs ball
void playerBallCollision(struct Game *game);

// initializes the game
void initGame(struct Game *game);

//performs a death
void initiateDeath(struct Game *game);

//Returns true if the player has run out of lives
bool isGameOver(struct Game *game);

//Returns true if the player has finished the last level and beat the game
bool hasBeatenGame(struct Game *game);

//returns a random float between 0-1
float randF();
