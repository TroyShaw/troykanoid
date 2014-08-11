#include <stdlib.h>

#include "fps.h"
#include "game_entities.h"
#include "physics.h"
#include "powerups.h"
#include "util.h"

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
        //if (rectToRect(paddle->x, paddle->y, paddle->width, paddle->height, p->x, p->y, p->width, p->height))
        if (false)
        {
            //printf("powerup: %s\n", powerup_name(p->type));
            p->type = MultiplyBall;
            switch (p->type)
            {
            case PaddleIncrease:
                //paddle->realWidth = min(paddle->realWidth + PADDLE_CHANGE, MAX_PADDLE_SIZE);
                break;
            
            case PaddleDecrease:
                //paddle->realWidth = max(paddle->realWidth - PADDLE_CHANGE, MIN_PADDLE_SIZE);
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
        //paddle->width++;
        //paddle->x -= 0.5f;
    }
    else if (paddle->width > paddle->realWidth)
    {
        //paddle->width--;
        //paddle->x += 0.5f;
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

void double_balls(struct Game *game)
{
    int numNewBalls = g_slist_length(game->ballList);

    GSList *newBallList = NULL;

    for (GSList *l = game->ballList; l != NULL; l = l->next)
    {
        struct Ball *oldBall = l->data;

        cpVect oldBallPos = cpBodyGetPos(oldBall->ballBody);
        cpVect oldBallVel = cpBodyGetVel(oldBall->ballBody);

        struct Ball *newBall = init_ball(game, oldBallPos.x + 30, oldBallPos.y);

        float newX = -oldBallVel.x;
        float newY = oldBallVel.y < 0 ? -oldBallVel.y : oldBallVel.y;

        cpBodySetVel(newBall->ballBody, cpv(newX, newY));
        //cpShapeSetSurfaceVelocity(newBall->ballShape, oldBallVel);
        //cpBodyApplyImpulse(newBall->ballBody, cpv(450, 300), cpv(0, 0));

        newBallList = g_slist_append(newBallList, newBall);
    }

    game->ballList = g_slist_concat(game->ballList, newBallList);

    game->numBalls += numNewBalls;


    //printf("made %d new balls\n", numNewBalls);
}

void generate_powerup(struct Game *game, int x, int y)
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
        p->type = (enum Powerups) (randF() * NUM_POWERUPS);
        p->inUse = true;

        //return since we only generate 1 powerup
        return;
    }
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
