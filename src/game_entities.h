#pragma once

#include <chipmunk/chipmunk.h>

#include "levels.h"
#include "powerups.h"

#define BALL_ARRAY_SIZE 256              //the max number, and size of the array, of balls on screen
#define BALL_RADIUS 8.0

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 40              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases

#define PADDLE_DEFAULT_WIDTH 100        //the default paddle size, and size it's set to on respawn
#define PADDLE_DEFAULT_HEIGHT 33.0

//These layer values are used by the physics engine to do conditional collisions
#define BALL_PADDLE_LAYER       0x00000001
#define BALL_WALL_LAYER         0x10000000
#define PADDLE_POWERUP_LAYER    0x00000010
#define PADDLE_DAMP_LAYER       0x00000100
#define UNUSED_BALL_LAYER       0x00001000
#define WALL_BUMPER_LAYER       0x00010000
#define BALL_BLOCK_LAYER        0x00100000
#define UNUSED_BLOCK_LAYER      0x01000000

//These are identifiers for each entity, used for the collision callbacks
#define BALL_COLLISION_TYPE  1
#define BLOCK_COLLISION_TYPE 2

//  ball (paddle, walls)
//  paddle (powerups, wall)

// ball, powerups, wall, paddle

//                balls         paddle      powerups    walls
//  ball            -              x                      x
//  paddle          -              -           x          x
//  powerups        -              -           -          
//  walls           -              -           -          -


struct Ball
{
    cpShape *ballShape;
    cpBody *ballBody;
    SDL_Color color;
    bool inUse;                         //true if ball is being displayed/ etc, on screen right now
};

struct Player
{
    int score;                          //score of player
    int lives;                          //number of lives the player has left. Getting 0 will mean game over
};


// Paddle is a rectangle with 2 circles for "bumpers" at each end.
// The paddle and bumpers have perfectly elastic collisions with the ball.
// The bumpers have a reasonably inelastic collision with the sides, so the user can better control the paddle.
// Thus we require duplicate bumpers for wall and ball collisions.
struct Paddle
{
    cpBody *paddleBody;
    cpShape *paddleShape;                       //this is the part the balls bounce on. Has elasticity of 1
    cpShape *leftBallBumper, *rightBallBumper;  //this is the part that hits the sides of the walls. Has elasticity of 0.3
    cpShape *leftWallBumper, *rightWallBumper;

    int width;
    int realWidth;
    int height;
    SDL_Color color;
};

struct Game
{
    cpSpace *space;
    cpShape *leftWall, *rightWall, *roof;       //walls of the level (ground not included!)

    cpShape *paddleDamper;

    bool paused;                                //true if we are in a game and paused, false we are not paused
    int currentLevel;                           //the current level the player is on
    struct Level level;                         //the level struct
    struct Player player;                       //the player
    struct Paddle paddle;
    struct Ball balls[BALL_ARRAY_SIZE];         //the balls
    int numBalls;                               //number of balls in use (useful for various reasons)
    bool attached;                              //if the ball is attached to paddle (i.e at start of games and after death)
    struct PowerupManager powerupManager;       //the manager for powerups
};
