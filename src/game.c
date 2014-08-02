#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "highscore.h"
#include "input.h"
#include "main.h"
#include "physics.h"

//Initialises the player paddle.
static void init_paddle(struct Game *game);

//Moves the player.
static void move_player(struct Game *game);

//Moves all the balls, (to be used when ball isn't attached). Keeps them within bounds of left/ right/ ceiling.
static void move_balls(struct Game *game);

//Moves the first ball to players location, should be used when ball is attached.
static void move_ball_to_player(struct Game *game);

//Test collision of balls and blocks.
static void ball_block_collisions(struct Game *game);

//Test collision of player vs ball.
static void player_ball_collision(struct Game *game);

//Goes to the next level.
static void goto_next_level(struct Game *game);

//Performs a death.
static void initiate_death(struct Game *game);

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

void tick_game(struct Game *game)
{
    cpVect paddlePos = cpBodyGetPos(game->paddle.paddleBody);

    printf("paddle x: %f, width: %d\n", paddlePos.x, game->paddle.width);

    //do this here so if they pushed space this tick, they get processed immediately
    if (game->attached && space_pressed())
    {
        //user pressed go! start the round

        game->attached = false;
        
        //shoot the ball away
        cpBodyApplyImpulse(game->balls[0].ballBody, cpv(450, 300), cpv(0, 0));
    } 

    //do a 1/60 timestep, but in 2 goes for better accuracy
    cpFloat timeStep = 1.0 / 120.0;
    cpSpaceStep(game->space, timeStep);
    cpSpaceStep(game->space, timeStep);

    if (game->attached)
    {
        move_player(game);
        move_ball_to_player(game);
    }
    else
    {
        move_player(game);
        move_balls(game);
        moveAndProcessPowerups(&game->powerupManager, game);
        ball_block_collisions(game);
        player_ball_collision(game);

        if (game->numBalls == 0) initiate_death(game);
        if (game->level.blocksLeft == 0) goto_next_level(game);
    }
}

static void move_player(struct Game *game)
{
    struct Paddle *paddle = &game->paddle;

    //will be -1 if left, 0 if left + right, 1 if right, 0 if neither
    int x = 0;
    int y = 0;

    if (dir_key_held(Left))  x -= 1;
    if (dir_key_held(Right)) x += 1;
    
    if (dir_key_held(Up))  y += 1;
    if (dir_key_held(Down)) y -= 1;

    float impulse = 50;
    float gravityImpulse = 20;

    cpBodyApplyImpulse(game->paddle.paddleBody, cpv(x * impulse, - gravityImpulse), cpvzero);
    cpBodyApplyImpulse(game->paddle.paddleBody, cpv(0, y * impulse), cpvzero);

    //cpVect force = cpBodyGetForce(game->paddle.paddleBody);
    //cpBodySetForce(game->paddle.paddleBody, force);

    //move paddle
    //paddle->x += x * PLAYER_MOVE_SPEED;

    //if paddle out of bounds of screen, move them within
    //paddle->x = min(max(paddle->x, 0), WIDTH - paddle->width);
}

static void move_balls(struct Game *game)
{
    struct Ball *ball = NULL;

    for (int i = 0; i < game->numBalls; i++)
    {
        ball = &game->balls[i];

        //do bounds checking.
        //if out of bottom, make ball unused and subtract 1 from ball total
        //else if out of other invert that directions velocity and set new position
        //TODO: will need to implement this

        // if (ball->x < 0)
        // {
        //     ball->x = 0;
        //     ball->velX *= -1.0f;
        // }
        // else if (ball->x + 2 * ball->radius > WIDTH)
        // {
        //     ball->x = WIDTH - 2 * ball->radius;
        //     ball->velX *= -1.0f;
        // }

        //TODO: forcefield powerup
        //Physics engine can calculuate this, but will need to bind a function that is called when collision happens        
        // if (game->powerupManager.forceField && ball->y < 10)
        // {
        //     ball->y = 10;
        //     ball->velY *= -1;
        //     game->powerupManager.forceField = false;
        // }

        //TODO: this logic checks if a ball goes below the board, and removes it from play if so, etc.
        // if (ball->y < 0)
        // {
        //     //TODO: hmm, this is weird
        //     //it seems I move balls around if they go outside the screen... not sure why
        //     game->balls[i] = game->balls[max(game->numBalls - 1, 0)];
        //     game->balls[max(game->numBalls - 1, 0)].inUse = false;
        //     game->numBalls--;

        //     //if there are no balls left, no point iterating over the rest
        //     //also if i == game->numBalls otherwise we'll iterate over the last (and dead) ball
        //     if (game->numBalls == 0 || i == game->numBalls) return;
        //     //otherwise we have to redo the loop with this new ball so it's processed
        //     else goto redo;
        // }
    }
}

static void move_ball_to_player(struct Game *game)
{
    struct Ball *ball = &game->balls[0];
    struct Paddle *paddle = &game->paddle;

    //TODO: need to attach the ball to the player somehow
    //ball->y = paddle->y + paddle->height;
    //ball->x = paddle->x + paddle->width / 2 - ball->radius;
}

static void initiate_death(struct Game *game)
{
    //subtract a life
    game->player.lives--;
    
    //if we have -1 lives, means it's gameover
    if (game->player.lives == -1) return;

    //reset paddle size and position
    //game->paddle.realWidth = PADDLE_DEFAULT_WIDTH;
    //game->paddle.width = PADDLE_DEFAULT_WIDTH;
    //game->paddle.x = (WIDTH - game->paddle.width) / 2;

    //set num balls back to 1
    game->numBalls = 1;
    //set first ball to on
    game->balls[0].inUse = true;
    //attach it to paddle
    game->attached = true;
    //move the ball to the player
    move_ball_to_player(game);
    
    //remove powerups
    for (int i = 0; i < NUM_POWERUPS; i++)
    {
        game->powerupManager.powerups[i].inUse = false;
    }
}

static void goto_next_level(struct Game *game)
{
    //we just beat the final level, so gameover
    if (game->currentLevel == NUM_LEVELS) return;

    //reset balls/ powerups/ paddle/ load new level
    game->currentLevel++;
    populate_level(&game->level, game->currentLevel);
}

static void ball_block_collisions(struct Game *game)
{
    int x, y, xV, yV;
    struct Block *bl = NULL;
    struct Ball *b = NULL;
    
    //TODO: this loop basically can be removed
    //need to do callbacks for blocks hitting balls though
    for (int i = 0; i < game->numBalls; i++)
    {
        //destroy blocks now
        for (int j = -1; j < 2; j++)
        {
            for (int k = -1; k < 2; k++)
            {
                // int ix = j + x;
                // int iy = k + y;
                
                // if (!collided[j + 1][k + 1] || ix < 0 || iy < 0 || ix >= BLOCKS_ACROSS || iy >= BLOCKS_DOWN) continue;

                // bl = &game->level.blocks[ix][iy];

                // if (!bl->indestructable)
                // {
                //     //ball wasn't indestructable, so set it to not used, subtract from total blocks, increment score
                //     bl->inUse = false;
                //     game->level.blocksLeft--;
                //     game->player.score += bl->points;
                //     if (randF() < POWERUP_PROB) generatePowerup(game, bl->x + bl->width / 2, bl->y + bl->height);
                // }
            }
        }
    }
}

static void player_ball_collision(struct Game *game)
{
    //TODO: basically this whole function can be removed I think

    // struct Paddle *paddle = &game->paddle;

    // for (int i = 0; i < game->numBalls; i++)
    // {
    //     struct Ball *ball = &game->balls[i];

    //     if (rectToRect(paddle->x, paddle->y, paddle->width, paddle->height, ball->x, ball->y, ball->radius * 2, ball->radius * 2))
    //     {
    //         float pos = (ball->x + ball->radius - paddle->x) / (float) paddle->width;
            
    //         if (pos < 0) pos = 0;
    //         if (pos > 1) pos = 1;

    //         ball->velY *= -1;
    //         ball->velX = 5.0f * (2.0f * pos - 1.0f);
    //         ball->y = paddle->y + paddle->height;
    //     }
    // }
}

void init_game(struct Game *game)
{
    //Initialise the game space
    cpVect gravity = cpv(0, 0);
    game->space = cpSpaceNew();
    cpSpaceSetGravity(game->space, gravity);
    //cpSpaceSetDamping(game->space, 0.05);

    //Initialise the walls
    game->leftWall = cpSegmentShapeNew(game->space->staticBody, cpv(0, HEIGHT), cpv(0, 0), 0);
    game->rightWall = cpSegmentShapeNew(game->space->staticBody, cpv(WIDTH, HEIGHT), cpv(WIDTH, 0), 0);
    game->roof = cpSegmentShapeNew(game->space->staticBody, cpv(0, HEIGHT), cpv(WIDTH, HEIGHT), 0);

    cpShapeSetFriction(game->leftWall, 1);
    cpShapeSetFriction(game->rightWall, 1);
    cpShapeSetFriction(game->roof, 1);

    cpShapeSetElasticity(game->leftWall, 1);
    cpShapeSetElasticity(game->rightWall, 1);
    cpShapeSetElasticity(game->roof, 1);

    cpSpaceAddShape(game->space, game->leftWall);
    cpSpaceAddShape(game->space, game->rightWall);
    cpSpaceAddShape(game->space, game->roof);

    cpShapeSetLayers(game->leftWall,  BALL_WALL_LAYER | WALL_BUMPER_LAYER);
    cpShapeSetLayers(game->rightWall, BALL_WALL_LAYER | WALL_BUMPER_LAYER);
    cpShapeSetLayers(game->roof, BALL_WALL_LAYER);

    //TODO move this elsewhere
    int paddleHeight = 15;
    //int paddleY = 10;
    int paddleY = paddleHeight;

    //Initialise the balls
    for (int i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        struct Ball *ball = &game->balls[i];

        ball->color = (SDL_Color) {255, 179, 179, 179};
        ball->inUse = false;

        cpFloat mass = 1;
        cpFloat moment = cpMomentForCircle(mass, 0, BALL_RADIUS, cpvzero);
        ball->ballBody = cpSpaceAddBody(game->space, cpBodyNew(mass, moment));

        int ballX = (WIDTH - BALL_RADIUS * 2) / 2;
        int ballY = paddleY + paddleHeight;
        
        cpBodySetPos(ball->ballBody, cpv(ballX, ballY));
        
        ball->ballShape = cpSpaceAddShape(game->space, cpCircleShapeNew(ball->ballBody, BALL_RADIUS, cpvzero));
        cpShapeSetFriction(ball->ballShape, 0.9);
        cpShapeSetElasticity(ball->ballShape, 1);

        //set balls to same group
        //TODO: make this number something else
        cpShapeSetGroup(ball->ballShape, 1);

        cpShapeSetLayers(ball->ballShape, UNUSED_BALL_LAYER);
    }

    init_paddle(game);
}

static void init_paddle(struct Game *game)
{
    //Initialise the paddle

    int paddleY = 10;
    int paddleX = (WIDTH - PADDLE_DEFAULT_WIDTH) / 2;

    cpFloat paddleMass = 1;
    
    //make the main body. Rotational inertia of INFINITY to stop it spinning
    game->paddle.paddleBody = cpSpaceAddBody(game->space, cpBodyNew(paddleMass, INFINITY));
    
    cpBodySetVelLimit(game->paddle.paddleBody, 500);
    cpBodySetPos(game->paddle.paddleBody, cpv(paddleX, paddleY));

    //initialise the main body of the paddle 
    game->paddle.paddleShape = cpSpaceAddShape(game->space, cpBoxShapeNew(game->paddle.paddleBody, PADDLE_DEFAULT_WIDTH, PADDLE_DEFAULT_HEIGHT));
    cpShapeSetLayers(game->paddle.paddleShape, BALL_PADDLE_LAYER | PADDLE_DAMP_LAYER);

    cpShapeSetFriction(game->paddle.paddleShape, 0.7);
    cpShapeSetElasticity(game->paddle.paddleShape, 1);

    //initialise the side bumpers
    float bumperRadius = PADDLE_DEFAULT_HEIGHT / 2;

    float bumperY = paddleY - PADDLE_DEFAULT_HEIGHT / 2;
    float bumperLeftX = paddleX;
    float bumperRightX = paddleX + PADDLE_DEFAULT_WIDTH;

    float xOffset = PADDLE_DEFAULT_WIDTH / 2;

    game->paddle.leftBallBumper = cpSpaceAddShape(game->space, cpCircleShapeNew(game->paddle.paddleBody, bumperRadius, cpv(-xOffset, 0)));
    game->paddle.rightBallBumper = cpSpaceAddShape(game->space, cpCircleShapeNew(game->paddle.paddleBody, bumperRadius, cpv(xOffset, 0)));

    game->paddle.leftWallBumper = cpSpaceAddShape(game->space, cpCircleShapeNew(game->paddle.paddleBody, bumperRadius, cpv(-xOffset, 0)));
    game->paddle.rightWallBumper = cpSpaceAddShape(game->space, cpCircleShapeNew(game->paddle.paddleBody, bumperRadius, cpv(xOffset, 0)));

    cpShapeSetLayers(game->paddle.leftWallBumper, WALL_BUMPER_LAYER);
    cpShapeSetLayers(game->paddle.rightWallBumper, WALL_BUMPER_LAYER);
    cpShapeSetFriction(game->paddle.leftWallBumper, 0.7);
    cpShapeSetFriction(game->paddle.rightWallBumper, 0.7);

    cpShapeSetLayers(game->paddle.leftBallBumper, BALL_PADDLE_LAYER);
    cpShapeSetLayers(game->paddle.rightBallBumper, BALL_PADDLE_LAYER);
    cpShapeSetFriction(game->paddle.leftBallBumper, 1);
    cpShapeSetFriction(game->paddle.rightBallBumper, 1);

    //Initialise paddle damper
    game->paddleDamper = cpSegmentShapeNew(game->space->staticBody, cpv(0, paddleY - PADDLE_DEFAULT_HEIGHT + 10), cpv(WIDTH, paddleY - PADDLE_DEFAULT_HEIGHT + 10), 0);
    cpShapeSetLayers(game->paddleDamper, PADDLE_DAMP_LAYER);
    cpShapeSetFriction(game->paddleDamper, 1);

    cpSpaceAddShape(game->space, game->paddleDamper);
}

void cleanup_game(struct Game *game)
{

}

void reset_game(struct Game *game)
{
    game->paused = false;

    game->paddle.color = (SDL_Color) {128, 128, 128, 128};
    game->paddle.width = PADDLE_DEFAULT_WIDTH;
    game->paddle.realWidth = PADDLE_DEFAULT_WIDTH;
    game->paddle.height = PADDLE_DEFAULT_HEIGHT;
    //game->paddle.x = (WIDTH - PADDLE_DEFAULT_WIDTH) / 2;
    //game->paddle.y = 10;
    game->paddle.height = 15;
    game->player.lives = DEFAULT_LIVES;
    game->player.score = 0;
    game->currentLevel = 1;

    for (int i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        struct Ball *ball = &game->balls[i];

        ball->color = (SDL_Color) {255, 179, 179, 179};
        
        //ball->x = (WIDTH - ball->radius * 2) / 2;
        //ball->y = game->paddle.y + game->paddle.height;
        // ball->velX = 5;
        // ball->velY = 5;
        // ball->inUse = false;
    }

    game->numBalls = 1;
    game->balls[0].inUse = true;
    cpShapeSetLayers(game->balls[0].ballShape, BALL_PADDLE_LAYER | BALL_WALL_LAYER);

    game->attached = true;

    reset_powerup_manager(&game->powerupManager);

    //load level 1
    populate_level(&game->level, 1);
}

bool is_game_over(struct Game *game)
{
    return game->player.lives == -1;
}

bool has_beaten_game(struct Game *game)
{
    return game->currentLevel == NUM_LEVELS;
}
