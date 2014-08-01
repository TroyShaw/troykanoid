#include <stdlib.h>

#include "game_entities.h"
#include "physics.h"
#include "powerups.h"
#include "util.h"

void doubleBalls();

void moveAndProcessPowerups(struct PowerupManager *manager, struct Game *game)
{
    struct Player *player = &game->player;
    struct Paddle *paddle = &game->paddle;
    int i = 0;
    struct Powerup *p;

    //iterate over whole range
    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        //get powerup
        p = &manager->powerups[i];
        if (!p->inUse) continue;

        //if we've collided, do some logic
        if (rectToRect(paddle->x, paddle->y, paddle->width, paddle->height, p->x, p->y, p->width, p->height))
        {
            switch (p->type)
            {
            case PaddleIncrease:
                paddle->realWidth += PADDLE_CHANGE;
                if (paddle->realWidth > MAX_PADDLE_SIZE) paddle->realWidth = MAX_PADDLE_SIZE;
                break;
            case PaddleDecrease:
                paddle->realWidth -= PADDLE_CHANGE;
                if (paddle->realWidth < MIN_PADDLE_SIZE) paddle->realWidth = MIN_PADDLE_SIZE;
                break;
            case BallSpeedup:
                break;
            case BallSlowdown:
                break;
            case BallComet:
                manager->meteor = true;
                manager->meteorCount = METEOR_COUNTDOWN;
                break;
            case MultiplyBall:
                doubleBalls();
                break;
            case ForceField:
                manager->forceField = true;
                manager->forceFieldCount = FORCE_FIELD_COUNTDOWN;
                break;
            case ExtraLife:
                player->lives++;
                break;
            }

            //increment the score
            player->score += POWERUP_POINTS;
            //show powerup not in use now
            p->inUse = false;
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

void doubleBalls(struct Game *game)
{
    //return if we have no room left for balls
    if (BALL_ARRAY_SIZE == game->numBalls) return;
    int numNewBalls = min(game->numBalls, BALL_ARRAY_SIZE - game->numBalls);
    int i;

    //clone our balls and invert x velocity
    for (i = 0; i < numNewBalls; i++)
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
        ;
    //glColor3f(0.2f, 0.9f, 0.2f);
    //glColor3f(randF(), randF(), randF());
}
