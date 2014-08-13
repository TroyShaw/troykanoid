#include <stdlib.h>

#include "fps.h"
#include "game_entities.h"
#include "physics.h"
#include "powerups.h"
#include "util.h"

//Allocates memory for the given powerup at that location
struct Powerup *internal_generate_powerup(struct Game *game, float x, float y);

//Frees the powerup
void free_powerup(struct Powerup *powerup);

void reset_powerup_manager(struct PowerupManager *manager)
{
    for (GSList *l = manager->powerups; l != NULL; l = l->next)
    {
        free_powerup(l->data);
    }

    g_slist_free(manager->powerups);
    manager->powerups = NULL;

    manager->forceField = false;
    manager->forceFieldCount = 0;

    manager->meteor = false;
    manager->meteorCount = 0;

    manager->sticky = false;
    manager->stickyCount = 0;
}

void move_and_process_powerups(struct PowerupManager *manager, struct Game *game)
{
    struct Player *player = &game->player;
    struct Paddle *paddle = &game->paddle;

    for (GSList *l = manager->powerups; l != NULL; l = l->next)
    {
        struct Powerup *p = l->data;
        
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
                double_balls(game);
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
            //p->inUse = false;

            //record the time of pickup
            p->tick_pickedup = frames_game();
        }
        else
        {
            //else we drop the powerup
            //p->y -= POWERUP_DROP_SPEED;
            
            //if it's below the ground, we set it to not be in use
            //p->inUse = (p->y + p->height) > 0;
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

    //note I do not need to free this list since g_slist_concat uses the nodes directly
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

        newBallList = g_slist_append(newBallList, newBall);
    }

    game->ballList = g_slist_concat(game->ballList, newBallList);

    game->numBalls += numNewBalls;
}

void free_powerup(struct Powerup *powerup)
{
    cpSpace *space = cpBodyGetSpace(powerup->powerupBody);

    cpSpaceRemoveBody(space, powerup->powerupBody);
    cpSpaceRemoveShape(space, powerup->powerupShape);

    cpBodyFree(powerup->powerupBody);
    cpShapeFree(powerup->powerupShape);

    free(powerup);
}

struct Powerup *internal_generate_powerup(struct Game *game, float x, float y)
{
    struct Powerup* p = malloc(sizeof *p);

    p->type = (enum Powerups) (randF() * NUM_POWERUPS);
    p->width = 10;
    p->height = 10;
    p->tick_pickedup = 0;

    p->powerupBody = cpSpaceAddBody(game->space, cpBodyNew(1, INFINITY));
    
    p->powerupShape = cpSpaceAddShape(game->space, cpBoxShapeNew(p->powerupBody, p->width, p->height));
    cpBodySetPos(p->powerupBody, cpv(x, y));    
    //TODO: set this to something else
    cpShapeSetGroup(p->powerupShape, 8);
    //cpShapeSetCollisionType(game->paddle.paddleShape, PADDLE_COLLISION_TYPE);
    cpShapeSetLayers(p->powerupShape, POWERUP_PADDLE_LAYER);
    cpShapeSetUserData(p->powerupShape, p);

    cpBodyApplyImpulse(p->powerupBody, cpv(0, POWERUP_DROP_VELOCITY), cpvzero);

    return p;
}

void generate_powerup(struct Game *game, int x, int y)
{
    struct Powerup *p = internal_generate_powerup(game, x, y);

    game->powerupManager.powerups = g_slist_append(game->powerupManager.powerups, p);
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
