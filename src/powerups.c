#include <stdlib.h>

#include "fps.h"
#include "game_entities.h"
#include "physics.h"
#include "powerups.h"
#include "util.h"

static void doubleBalls(struct Game *game);

void reset_powerup_manager(struct PowerupManager *manager)
{
    for (int i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        struct Powerup* p = &manager->powerups[i];
        
        //set the width and height cause it isn't initialized anywhere else...
        //TODO: actually maybe it can be set on powerup creation..? 
        p->height = 5;
        p->width = 5;
        p->inUse = false;

        //set this number negative so that we don't render them at the start of the game
        p->tick_pickedup = -10000;
    }

    manager->forceField = false;
    manager->forceFieldCount = 0;

    manager->meteor = false;
    manager->meteorCount = 0;

    manager->sticky = false;
    manager->stickyCount = 0;
}

void moveAndProcessPowerups(struct PowerupManager *manager, struct Game *game)
{
    struct Player *player = &game->player;
    struct Paddle *paddle = &game->paddle;

    for (int i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        struct Powerup *p = &manager->powerups[i];
        
        if (!p->inUse) continue;

        //if we've collided, do some logic
        if (rectToRect(paddle->x, paddle->y, paddle->width, paddle->height, p->x, p->y, p->width, p->height))
        {
            //printf("powerup: %s\n", powerup_name(p->type));
            p->type = MultiplyBall;
            switch (p->type)
            {
            case PaddleIncrease:
                paddle->realWidth = min(paddle->realWidth + PADDLE_CHANGE, MAX_PADDLE_SIZE);
                break;
            
            case PaddleDecrease:
                paddle->realWidth = max(paddle->realWidth - PADDLE_CHANGE, MIN_PADDLE_SIZE);
                break;
            
            case BallSpeedup:
                break;
            
            case BallSlowdown:
                break;
            
            case MultiplyBall:
                //TODO: causes crash
                doubleBalls(game);
                break;
            
            case BallComet:
                manager->meteor = true;
                manager->meteorCount = METEOR_COUNTDOWN;
                break;
            
            case ForceField:
                manager->forceField = true;
                manager->forceFieldCount = FORCE_FIELD_COUNTDOWN;
                break;
            
            case StickyPaddle:
                manager->sticky = true;
                manager->stickyCount = STICKY_PADDLE_COUNTDOWN;

            case ExtraLife:
                player->lives++;
                break;
            }

            //all powerups increase score, so increment the score
            player->score += POWERUP_POINTS;

            //show powerup not in use now
            p->inUse = false;

            //record the time of pickup
            p->tick_pickedup = frames_game();
        }
        else
        {
            //else we drop the powerup
            p->y -= POWERUP_DROP_SPEED;
            
            //if it's below the ground, we set it to not be in use
            p->inUse = (p->y + p->height) > 0;
        }
    }

    //grow/shrink the player if they aren't their actual width size
    if (paddle->width < paddle->realWidth)
    {
        paddle->width++;
        paddle->x -= 0.5f;
    }
    else if (paddle->width > paddle->realWidth)
    {
        paddle->width--;
        paddle->x += 0.5f;
    }

    //decrement the forcefield
    if (manager->forceFieldCount > 0)
    {
        manager->forceFieldCount--;
        if (manager->forceFieldCount == 0) manager->forceField = false;
    }

    //decrement the meteor
    if (manager->meteor > 0)
    {
        manager->meteorCount--;
        if (manager->meteorCount == 0) manager->meteor = false;
    }
}

//numBalls
//                              BALL_ARRAY_SIZE = 10
// 0  1  2  3  4  5  6  7  8  9
// [] [] [] [] [] [] [] [] [] [] 

static void doubleBalls(struct Game *game)
{

    //return if we have no room left for balls
    if (BALL_ARRAY_SIZE == game->numBalls) return;
    //printf("Made here %d\n", i);
    int numNewBalls = min(game->numBalls, BALL_ARRAY_SIZE - game->numBalls);

    //clone our balls and invert x velocity
    for (int i = 0; i < numNewBalls; i++)
    {

        game->balls[game->numBalls + i] = game->balls[i];
        game->balls[game->numBalls + i].velX *= -1;
        //make ball always shoot upwards otherwise makes it hard to keep ball in play
        //when ball is descending`
        game->balls[game->numBalls + i].velY = abs(game->balls[game->numBalls + i].velY);

    }

    //then increment our ball count
    game->numBalls += numNewBalls;
}

void generatePowerup(struct Game *game, int x, int y)
{
    int w = 10, h = 10;
    int i;
    struct Powerup *p;

    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        p = &game->powerupManager.powerups[i];
        if (p->inUse) continue;

        p->width = w;
        p->height = h;
        p->x = x - w / 2;
        p->y = y - h / 2;
        p->type = randF() * NUM_POWERUPS;
        p->inUse = true;

        //return since we only generate 1 powerup
        return;
    }
}

void setPowerupColor(struct Game *game, int type)
{
    //TODO
    if (type || game)
    {
        ;
    }
    //glColor3f(0.2f, 0.9f, 0.2f);
    //glColor3f(randF(), randF(), randF());
}

const char* powerup_name(enum Powerups powerup)
{
    switch (powerup)
    {
        case PaddleIncrease: return "Paddle Grow!";
        case PaddleDecrease: return "Paddle Increase!";
        case BallSpeedup:    return "Ball Speedup!";
        case BallSlowdown:   return "Ball Slowdown!";
        case BallComet:      return "Comet!";
        case MultiplyBall:   return "Multiply Ball!";
        case ForceField:     return "Force-field!";
        case ExtraLife:      return "Extra Life!";
        case StickyPaddle:   return "Sticky Paddle!";
    }

    printf("unknown powerup reached\n");
    exit(1);
}
