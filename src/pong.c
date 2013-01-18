#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <GL/glut.h>
#include "pong.h"
#include "main.h"
#include "highscore.h"

void doubleBalls();

Game game;

//general algorithm
//two modes, attached, not attached
//attached:
//  move player
//  move ball position to player
//non attached:
//  move player
//  move balls
//  move powerups
//  check powerups collision with player, if so, apply poweurp
//  check ball collision with player, if so, make balls y velocity positive
//  check ball isn't out of bounds of screen, of so, set ball as not active, and subtract 1 from current balls
//      if current balls is 0, initiate death sequence
//          remove 1 from lives
//          if lives is -1, end game
//          else
//          make all powerups inactive
//          set first ball to active
//          set attached mode

void tickGame()
{

    if (!game.running)
    {
        if (game.Keymanager.space)
        {
            game.running = true;
            initGame();

            //do this so that the user needs to push space again to fire the ball
            game.Keymanager.space = false;
        }
        else
        {
            return;
        }
    }

    if (game.Keymanager.pause)
    {
        //set to false so we don't redo this next tick
        game.Keymanager.pause = false;
        //invert pause mode
        game.paused ^= true;
    }

    if (game.paused) return;

    //do this here so if they pushed space this tick, they get processed immediately
    if (game.attached) game.attached = !game.Keymanager.space;

    if (game.attached)
    {
        movePlayer();
        moveBallToPlayer();
    }
    else
    {
        movePlayer();
        moveBalls();
        moveAndProcessPowerups();
        playerBallCollision();
        if (game.numBalls == 0) initiateDeath();
    }
}

void moveAndProcessPowerups()
{
    PowerupManager *manager = &game.powerupManager;
    Player* player = &game.player;
    int i = 0;
    Powerup* p;
    //true if we are generating a powerup this tick
    bool newP = randF() < POWERUP_PROB;

    //iterate over whole range
    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        //get powerup
        p = &(manager->powerups[i]);
        if (!p->inUse)
        {
            if (newP)
            {
                //if not in use and we want a new one, we should generate it
                p->inUse = true;
                p->y = 400;
                p->width = 10;
                p->height = 10;
                p->x = randF() * WIDTH;
                p->type = randF() * NUM_POWERUPS;
                //then set flag to false so we don't generate another
                newP = false;
            }
            //otherwise continue loop
            continue;
        }

        //if we've collided, do some logic
        if (playerCollidePowerup(*p))
        {
            //switch (p->type)
            switch (MULT_BALL)
            {
            case PADDLE_INCREASE:
                player->realWidth += PADDLE_CHANGE;
                if (player->realWidth > MAX_PADDLE_SIZE) player->realWidth = MAX_PADDLE_SIZE;
                break;
            case PADDLE_DECREASE:
                player->realWidth -= PADDLE_CHANGE;
                if (player->realWidth < MIN_PADDLE_SIZE) player->realWidth = MIN_PADDLE_SIZE;
                break;
            case BALL_SPEEDUP:
                break;
            case BALL_SLOWDOWN:
                break;
            case BALL_COMET:
                break;
            case MULT_BALL:
                doubleBalls();
                break;
            case FORCE_FIELD:
                manager->forceFieldCount = FORCE_FIELD_COUNTDOWN;
                manager->forceField = true;
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
    if (player->width < player->realWidth)
    {
        player->width++;
        player->x -= 0.5f;
    }
    else if (player->width > player->realWidth)
    {
        player->width--;
        player->x += 0.5f;
    }

    //decrement the forcefield
    if (manager->forceFieldCount > 0)
    {
        manager->forceFieldCount--;
        if (manager->forceFieldCount == 0) manager->forceField = false;
    }
}

void doubleBalls()
{
    //return if we have no room left for balls
    if (BALL_ARRAY_SIZE == game.numBalls) return;
    int numNewBalls = min(game.numBalls, BALL_ARRAY_SIZE - game.numBalls);

    Ball tBalls[numNewBalls];
    int i, j , count = 0;

    //set to not in use
    for (i = 0; i < numNewBalls; i++) tBalls[i].inUse = false;

    //first we clone balls until we have enough
    for (i = 0; i < BALL_ARRAY_SIZE && count != numNewBalls; i++)
    {
        if (!game.balls[i].inUse) continue;

        //if ball is in use we copy it
        tBalls[count] = game.balls[i];
        count++;
    }

    //now we transfer cloned balls to the main ball array
    for (i = 0; i < numNewBalls; i++)
    {
        for (j = 0; j < BALL_ARRAY_SIZE; j++)
        {
            if (game.balls[j].inUse) continue;

            game.balls[j] = tBalls[i];
            game.balls[j].velX *= -1;
            break;
        }
    }

    //then increment our ball
    game.numBalls += numNewBalls;
}

void movePlayer()
{
    Player* player = &game.player;
    Keymanager kManager = game.Keymanager;

    //will be -1 if left, 0 if left + right, 1 if right, 0 if neither
    int x = kManager.right - kManager.left;

    //move player
    player->x += x * PLAYER_MOVE_SPEED;

    //if player out of bounds of screen, move them within
    player->x = min(max(player->x, 0), WIDTH - player->width);
}

void moveBalls()
{
    int i;
    Ball* ball = NULL;

    for (i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        ball = &(game.balls[i]);

        //continue if ball isn't being used
        if (!ball->inUse) continue;

        //increment velocity
        ball->x += ball->velX;
        ball->y += ball->velY;

        //do bounds checking.
        //if out of bottom, make ball unused and subtract 1 from ball total
        //else if out of other invert that directions velocity and set new position

        if (ball->x < 0)
        {
            ball->x = 0;
            ball->velX *= -1.0f;
        }
        else if (ball->x + 2 * ball->radius > WIDTH)
        {
            ball->x = WIDTH - 2 * ball->radius;
            ball->velX *= -1.0f;
        }

        if (game.powerupManager.forceField && ball->y < 10)
        {
            ball->y = 10;
            ball->velY *= -1;
            game.powerupManager.forceField = false;
        }

        if (ball->y < 0)
        {
            ball->inUse = false;
            game.numBalls--;

            //if there are no balls left, no point iterating over the rest
            if (game.numBalls == 0) return;
        }
        else if (ball->y + 2 * ball->radius > HEIGHT)
        {
            ball->y = HEIGHT - 2 * ball->radius;
            ball->velY *= -1;
        }
    }
}

void moveBallToPlayer()
{
    Ball* ball = &game.balls[0];
    Player* player = &game.player;

    ball->y = player->y + player->height;
    ball->x = player->x + player->width / 2 - ball->radius;
}

void initiateDeath()
{
    //subtract a life
    game.player.lives--;
    //if we have -1 lives, means it's gameover
    if (game.player.lives == -1)
    {
        enterScore("john", game.player.score);
        game.running = false;
        //then save highscore
        saveHighscoresToDisc();
        return;
    }

    //reset paddle size and position
    game.player.realWidth = PADDLE_DEFAULT_WIDTH;
    game.player.width = PADDLE_DEFAULT_WIDTH;
    game.player.x = (WIDTH - game.player.width) / 2;

    //set num balls back to 1
    game.numBalls = 1;
    //set first ball to on
    game.balls[0].inUse = true;
    //attach it to paddle
    game.attached = true;
    //move the ball to the player
    moveBallToPlayer();
    //remove powerups
    int i;
    for (i = 0; i < NUM_POWERUPS; i++)
    {
        game.powerupManager.powerups[i].inUse = false;
    }
}

bool playerCollidePowerup(Powerup p)
{
    Player pl = game.player;
    return collide(pl.x, pl.y, pl.width, pl.height, p.x, p.y, p.width, p.height);
}

void playerBallCollision()
{
    int i;
    Ball ball;
    Player player = game.player;

    for (i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        ball = game.balls[i];

        if (!ball.inUse) continue;

        if (collide(player.x, player.y, player.width, player.height, ball.x, ball.y, ball.radius * 2, ball.radius * 2))
        {
            float pos = (ball.x + ball.radius - player.x) / (float) player.width;
            if (pos < 0) pos = 0;
            if (pos > 1) pos = 1;

            ball.velY *= -1;
            ball.velX = 5.0f * (2.0f * pos - 1.0f);
            ball.y = player.y + player.height;
            game.balls[i] = ball;
        }
    }
}

//Initialises the game variables
void initGame()
{
    int i;

    game.paused = false;

    game.player.color = (Color)
    {
        0.5f, 0.5f, 0.5f, 0.5f
    };
    game.player.width = PADDLE_DEFAULT_WIDTH;
    game.player.realWidth = game.player.width;
    game.player.x = (WIDTH - game.player.width) / 2;
    game.player.y = 10;
    game.player.height = 15;
    game.player.lives = DEFAULT_LIVES;
    game.player.score = 0;

    Ball* ball;
    for (i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        ball = &game.balls[i];

        ball->color = (Color)
        {
            1.0f, 0.7f, 0.7f, 0.7f
        };
        ball->radius = 4;
        ball->x = (WIDTH - ball->radius * 2) / 2;
        ball->y = game.player.y + game.player.height;
        ball->velX = 5;
        ball->velY = 5;
        ball->inUse = false;
    }

    game.numBalls = 1;
    game.balls[0].inUse = true;

    game.attached = true;

    for (i = 0; i < POWERUP_ARRAY_SIZE; i++)
    {
        Powerup* p = &game.powerupManager.powerups[i];
        p->height = 5;
        p->inUse = false;
        p->width = 5;
    }
    game.powerupManager.forceField = false;
    game.powerupManager.forceFieldCount = 0;

    //controller stuff
    game.Keymanager.pause = false;

    //load level 1
    populateLevel(1);
}

void setPowerupColor(int type)
{
    //glColor3f(1.0f, 0.2f, 0.2f);
    glColor3f(randF(), randF(), randF());
}


float randF()
{
    return ((float) rand()) / RAND_MAX;
}

bool collide(int tx, int ty, int tw, int th, int rx, int ry, int rw, int rh)
{
    //no collision
    if (rw <= 0 || rh <= 0 || tw <= 0 || th <= 0) return false;

    rw += rx;
    rh += ry;
    tw += tx;
    th += ty;
    //      overflow || intersect
    return ((rw > tx) &&
            (rh > ty) &&
            (tw > rx) &&
            (th > ry));
}
