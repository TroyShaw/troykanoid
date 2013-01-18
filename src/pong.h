#pragma once

#include <stdbool.h>
#include "highscore.h"
#include "levels.h"

#define PLAYER_MOVE_SPEED 7             //the movement speed of the player in pixels per tick
#define PADDLE_DEFAULT_WIDTH 100        //the default paddle size, and size it's set to on respawn
#define DEFAULT_LIVES 3                 //the number of lives you get at start of game

#define BALL_ARRAY_SIZE 1024              //the max number, and size of the array, of balls on screen
#define MAX_BLOCKS 128                  //the max number, and size of the array, of blocks the player can destroy
#define POWERUP_ARRAY_SIZE 32           //the max number, and size of the array, of the powerups
#define POWERUP_DROP_SPEED 4            //the drop speed in pixels per game tick
#define POWERUP_PROB 0.005              //the probability of a powerup appearing in a tick
#define NUM_POWERUPS 7                  //the number of powerups defined (check below)
#define FORCE_FIELD_COUNTDOWN 600       //number of ticks it takes before forcefield goes away
#define POWERUP_POINTS 50               //amount of points you get for picking up a powerup

////////////////
//  POWERUPS  //
////////////////
#define PADDLE_INCREASE 0               //causes the paddle to increase by PADDLE_CHANGE, to a max of MAX_PADDLE_SIZE
#define PADDLE_DECREASE 1               //causes the paddle to decrease by PADDLE_CHANGE, to a min of MIN_PADDLE_SIZE
#define BALL_SPEEDUP 2                  //causes the ball to speed up
#define BALL_SLOWDOWN 3                 //causes the ball to slow down
#define BALL_COMET 4                    //causes the ball to become a comet for a duration, which means it destroys blocks instantly
#define MULT_BALL 5                     //causes the number of balls on screent to double (to a max of BALL_ARRAY_SIZE
#define FORCE_FIELD 6                   //causes a forcefield to appear below the player which rebounds a single ball

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 10              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases
#define PADDLE_CHANGE 40                //the amount of pixels the paddle increases/ decreases by when picking up that powerup

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
    int width;                          //current width of paddle
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
    bool running;                       //true if running, false if on menu screen
    bool paused;                        //true if we are in a game and paused, false we are not paused
    int currentLevel;                   //the current level the player is on
    Player player;                      //the player
    Ball balls[BALL_ARRAY_SIZE];        //the balls
    int numBalls;                       //number of balls in use (useful for various reasons)
    bool attached;                      //if the ball is attached to paddle (i.e at start of games and after death)
    PowerupManager powerupManager;      //the manager for powerups
    HighscoreManager highscoreManager;  //the highscore manager
    Keymanager Keymanager;              //the key manager, holds keystate
    Block blocks[BLOCKS_ACROSS][BLOCKS_DOWN];//all the blocks
} Game;


//ticks the game. Should be done at around 30 times per second
void tickGame();
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
//returns true if player collided with that powerup
bool playerCollidePowerup(Powerup p);
// initializes the game
void initGame();
//generates powerups
void moveAndProcessPowerups();
//performs a death
void initiateDeath();
//sets gl's color to the given powerup types
void setPowerupColor(int type);
//returns a random float between 0-1
float randF();
//returns if rectangle t collides with rectangle r
bool collide(int tx, int ty, int tw, int th, int rx, int ry, int rw, int rh);

extern Game game;
