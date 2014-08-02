#pragma once

#include <stdbool.h>

//TODO: currently a powerup can be displaying it's grab text, and the code to generate a new powerup will select it to drop
//meaning that the text will suddenly disappear if the ball happens to hit another block and a powerup is generated
//need to add in a check for this (maybe states the powerup can be in)

#define POWERUP_ARRAY_SIZE 32           //the max number, and size of the array, of the powerups

#define POWERUP_DROP_SPEED 4            //the drop speed in pixels per game tick
#define POWERUP_PROB 0.8                //the probability of a powerup appearing on block destruction
#define POWERUP_POINTS 50               //amount of points you get for picking up a powerup
#define FORCE_FIELD_COUNTDOWN 600       //number of ticks it takes before forcefield goes away
#define METEOR_COUNTDOWN 600            //number of ticks it takes before meteor powerup goes away
#define STICKY_PADDLE_COUNTDOWN 600     //number of ticks it takes before sticky paddle goes away
#define PADDLE_CHANGE 40                //the amount of pixels the paddle increases/ decreases by when picking up that powerup

////////////////
//  POWERUPS  //
////////////////
enum Powerups
{
    PaddleIncrease,     //causes the paddle to increase by PADDLE_CHANGE, to a max of MAX_PADDLE_SIZE
    PaddleDecrease,     //causes the paddle to decrease by PADDLE_CHANGE, to a min of MIN_PADDLE_SIZE
    BallSpeedup,        //causes the ball to speed up
    BallSlowdown,       //causes the ball to slow down
    BallComet,          //causes the ball to become a comet for a duration, which means it destroys blocks instantly
    MultiplyBall,       //causes the number of balls on screent to double (to a max of BALL_ARRAY_SIZE
    ForceField,         //causes a forcefield to appear below the player which rebounds a single ball
    ExtraLife,          //gives the player an extra life
    StickyPaddle         //causes the ball to stick to the paddle until user pushes space to release them
};

#define NUM_POWERUPS 9

struct Powerup
{
    int x;                              //x coord of powerup
    int y;                              //y coord of powerup
    int width;                          //width of powerup
    int height;                         //height of powerup
    enum Powerups type;                 //type of powerup. Check above for different types
    bool inUse;                         //true if we are displaying powerup on screen
    long int tick_pickedup;             //the frames_game() we picked this powerup at
};

struct PowerupManager
{
    struct Powerup powerups[POWERUP_ARRAY_SIZE];    //the array of all powerups.
    bool forceField;                                //true if the user has activated the forcefield
    int forceFieldCount;                            //ticks left the field has
    bool meteor;                                    //true if the balls are in meteor mode (go straight through destructible blocks)
    int meteorCount;                                //ticks left the meteor has
    bool sticky;                                    //true if the balls stick to paddle
    int stickyCount;                                 //ticks left sticky has
};

//forward declaration of Game struct
//TODO: can we remove this?
struct Game;

//Resets the powerup-manager to a fresh state. Should be called at the start of each level.
void reset_powerup_manager(struct PowerupManager *manager);

void moveAndProcessPowerups(struct PowerupManager *manager, struct Game *game);          //moves the powerups. If there
void setPowerupColor(struct Game *game, int type);         //sets opengl's color to the balls color

//generates a new powerup (assumes a block has been destroyed and probability determined to generate one)
//x and y are the middle of the block in which the powerup was destroyed. It needs to be adjusted
//for the width of the powerup too
void generatePowerup(struct Game *game, int x, int y);      //generates a random powerup at the given (x,y) coordinates

const char* powerup_name(enum Powerups powerup);
