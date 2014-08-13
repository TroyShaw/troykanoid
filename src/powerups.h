#pragma once

#include <stdbool.h>
#include <glib.h>

#define POWERUP_DROP_VELOCITY -200      //the drop speed in pixels per game tick
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
    StickyPaddle        //causes the ball to stick to the paddle until user pushes space to release them
};

#define NUM_POWERUPS 9

struct Powerup
{
    struct cpShape *powerupShape;
    struct cpBody *powerupBody;
    int width;                          //width of powerup
    int height;                         //height of powerup
    enum Powerups type;                 //type of powerup. Check above for different types
    long int tick_pickedup;             //the frames_game() we picked this powerup at
};

struct PowerupManager
{
    GSList *powerups;                               //the powerups
    bool forceField;                                //true if the user has activated the forcefield
    int forceFieldCount;                            //ticks left the field has
    bool meteor;                                    //true if the balls are in meteor mode (go straight through destructible blocks)
    int meteorCount;                                //ticks left the meteor has
    bool sticky;                                    //true if the balls stick to paddle
    int stickyCount;                                //ticks left sticky has
};

//forward declaration of the Game struct
struct Game;

//Resets the powerup-manager to a fresh state. Should be called at the start of each level.
void reset_powerup_manager(struct PowerupManager *manager);

void move_and_process_powerups(struct PowerupManager *manager, struct Game *game);

//generates a random powerup at the given (x,y) coordinates
void generate_powerup(struct Game *game, int x, int y);      

//Gets the powerups name.
const char* powerup_name(enum Powerups powerup);

void double_balls(struct Game *game);