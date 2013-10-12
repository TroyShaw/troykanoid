#pragma once

#include <stdbool.h>
#include "highscore.h"
#include "levels.h"
#include "powerups.h"

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

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
#define POWERUP_ARRAY_SIZE 32           //the max number, and size of the array, of the powerups

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 40              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases
#define PADDLE_CHANGE 40                //the amount of pixels the paddle increases/ decreases by when picking up that powerup

//NOTE: coordinate system starts at bottom left (x, y) and goes positive right x, positive up y

typedef struct
{
    float r;                            //red component
    float g;                            //green component
    float b;                            //blue component
    float a;                            //alpha component
} Color;

typedef struct
{
    float x;                              //x coord
    float y;                              //y coord
    int radius;                         //radius of ball
    Color color;                        //its color
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
    Color color;                        //color of paddle
} Player;

typedef struct
{
    int x;                              //x coord of powerup
    int y;                              //y coord of powerup
    int width;                          //width of powerup
    int height;                         //height of powerup
    int type;                           //type of powerup. Check above for different types
    bool inUse;                         //true if we are displaying powerup on screen
} Powerup;

typedef struct
{
    Powerup powerups[POWERUP_ARRAY_SIZE];   //the array of all powerups.
    bool forceField;                        //true if the user has activated the forcefield
    int forceFieldCount;                    //ticks left the field has
    bool meteor;                            //true if the balls are in meteor mode (go straight through destructable blocks)
    int meteorCount;                        //ticks left the meteor has
    bool sticky;                            //true if the balls stick to paddle
    int stickCount;                         //ticks left sticky has
} PowerupManager;

typedef struct
{
    int x;                                  //the absolute x position of this block
    int y;                                  //the absolute y position of this block
    int width;                              //the width of this block
    int height;                             //the height of this block
    int points;                             //the number of points for destroying this block
    Color color;                            //the color of this block
    int type;                               //the type of this block
    int hitsLeft;                           //the "health" of this block, how many more hits required before it dies
    bool indestructable;                    //if this block is indestructable; balls just bounce off it
    bool inUse;                             //true if we are displaying this
} Block;

typedef struct
{
    bool left;                              //true if left is currently held
    bool right;                             //true if right is currently held
    bool space;                             //true if space is currently held
    bool pause;                             //true if the pause button is currently held
} Keymanager;

typedef struct
{
    GameMode mode;                      //the mode of the game
    bool paused;                        //true if we are in a game and paused, false we are not paused
    int currentLevel;                   //the current level the player is on
    int blocksLeft;                     //the amount of breakable blocks left on this level
    Player player;                      //the player
    Ball balls[BALL_ARRAY_SIZE];        //the balls
    int numBalls;                       //number of balls in use (useful for various reasons)
    bool attached;                      //if the ball is attached to paddle (i.e at start of games and after death)
    PowerupManager powerupManager;      //the manager for powerups
    HighscoreManager highscoreManager;  //the highscore manager
    Keymanager keymanager;              //the key manager, holds keystate
    Block blocks[BLOCKS_ACROSS][BLOCKS_DOWN];//all the blocks
} Game;


//ticks the game. Should be done at around 30 times per second
void tick();
//moves the player
void movePlayer();
//moves all the ball, (to be used when ball isn't attached). Keeps them within bounds of left/right/ceiling
void moveBalls();
//moves the first ball to players location, should be used when ball is attached
void moveBallToPlayer();
//test collision of balls and blocks
void ballBlockCollisions();
//test collision of player vs ball
void playerBallCollision();
// initializes the game
void initGame();
//performs a death
void initiateDeath();
//returns a random float between 0-1
float randF();

extern Game game;
