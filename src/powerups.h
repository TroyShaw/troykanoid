#pragma once

#define POWERUP_ARRAY_SIZE 32           //the max number, and size of the array, of the powerups

#define POWERUP_DROP_SPEED 4            //the drop speed in pixels per game tick
#define POWERUP_PROB 0.2                //the probability of a powerup appearing on block destruction
#define POWERUP_POINTS 50               //amount of points you get for picking up a powerup
#define FORCE_FIELD_COUNTDOWN 600       //number of ticks it takes before forcefield goes away
#define METEOR_COUNTDOWN 600            //number of ticks it takes before meteor powerup goes away
#define STICKY_PADDLE_COUNTDOWN 600     //number of ticks it takes before sticky paddle goes away

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
#define EXTRA_LIFE 7                    //gives the player an extra life
#define STICKY_PADDLE 8                 //causes the ball to stick to the paddle until user pushes space to release them

#define NUM_POWERUPS 9                  //the number of powerups defined

typedef struct
{
    int x;                              //x coord of powerup
    int y;                              //y coord of powerup
    int width;                          //width of powerup
    int height;                         //height of powerup
    int type;                           //type of powerup. Check above for different types
    bool inUse;                         //true if we are displaying powerup on screen
} Powerup;

struct PowerupManager
{
    Powerup powerups[POWERUP_ARRAY_SIZE];   //the array of all powerups.
    bool forceField;                        //true if the user has activated the forcefield
    int forceFieldCount;                    //ticks left the field has
    bool meteor;                            //true if the balls are in meteor mode (go straight through destructable blocks)
    int meteorCount;                        //ticks left the meteor has
    bool sticky;                            //true if the balls stick to paddle
    int stickCount;                         //ticks left sticky has
};

void moveAndProcessPowerups(struct PowerupManager *manager);          //moves the powerups. If there
void setPowerupColor(int type);         //sets opengl's color to the balls color
void generatePowerup(int x, int y);      //generates a random powerup at the given (x,y) coordinates
