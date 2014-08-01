#pragma once

#include "levels.h"
#include "powerups.h"

#define BALL_ARRAY_SIZE 256              //the max number, and size of the array, of balls on screen

#define MAX_PADDLE_SIZE 350             //the maximum size of the paddle
#define MIN_PADDLE_SIZE 40              //the minimum size of the paddle
#define PADDLE_CHANGLE_SPEED 5          //the speed at which the paddle increases

struct Ball
{
    float x;                              //x coord
    float y;                              //y coord
    int radius;                         //radius of ball
    SDL_Color color;                        //its color
    float velX;                         //velocity in x axis
    float velY;                         //velocity in y axis
    bool inUse;                         //true if ball is being displayed/ etc, on screen right now
};

struct Player
{
    int score;                          //score of player
    int lives;                          //number of lives the player has left. Getting 0 will mean game over
};

struct Paddle
{
    float x;
    float y;
    int width;
    int realWidth;
    int height;
    SDL_Color color;
};

struct Game
{
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
