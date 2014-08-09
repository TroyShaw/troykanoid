#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "game.h"
#include "highscore.h"
#include "input.h"
#include "main.h"
#include "physics.h"

//Initialises the player paddle.
static void init_paddle(struct Game *game);

//Initialises the blocks
static void init_blocks(struct Game *game);

//Moves the player.
static void move_player(struct Game *game);

//Called at the start of a collision. Return value determines if calculations continue for this collision.
//We use this to allow for power balls to work properly.
static cpBool ball_block_begin(cpArbiter *arb, cpSpace *space, void *ignore);

//Called after a ball-block collision. Used to get rid of the block.
static void ball_block_post_solve(cpArbiter *arb, cpSpace *space, void *ignore);

//Called during the ball-paddle collision. Used to implement the proper angle reflections for collision.
static cpBool ball_paddle_pre_solve(cpArbiter *arb, cpSpace *space, void *ignore);

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
    //do this here so if they pushed space this tick, they get processed immediately
    if (game->attached && space_pressed())
    {
        //user pressed go! start the round
        game->attached = false;
        
        //shoot the ball away
        struct Ball *b = game->ballList->data;
        cpBodyApplyImpulse(b->ballBody, cpv(450, 300), cpv(0, 0));
    } else if (!game->attached && space_pressed())
    {
        double_balls(game);
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
    //will be -1 if left, 0 if left + right, 1 if right, 0 if neither
    int x = 0;
    int y = 0;

    if (dir_key_held(Left))  x -= 1;
    if (dir_key_held(Right)) x += 1;
    
    if (dir_key_held(Up))  y += 1;
    if (dir_key_held(Down)) y -= 1;

    float impulse = 80;
    float gravityImpulse = 28;

    cpBodyApplyImpulse(game->paddle.paddleBody, cpv(x * impulse, -gravityImpulse), cpvzero);
    //cpBodyApplyImpulse(game->paddle.paddleBody, cpv(0, y * impulse), cpvzero);
}

static void move_balls(struct Game *game)
{
    for (GList *l = game->ballList; l != NULL; l = l->next)
    {
        struct Ball *ball = l->data;

        if (ball)
        {
            //TODO
            ;
        }

        //do bounds checking.
        //if out of bottom, make ball unused and subtract 1 from ball total
        //else if out of other invert that directions velocity and set new position
        //TODO: will need to implement this


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
    struct Ball *ball = game->ballList->data;
    struct Paddle *paddle = &game->paddle;

    cpVect pos = cpBodyGetPos(paddle->paddleBody);

    //TODO: need to attach the ball to the player somehow
    cpBodySetPos(ball->ballBody, cpv(pos.x, pos.y + paddle->height / 2.0 + BALL_RADIUS));
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
    struct Ball *ball = game->ballList->data;
    ball->inUse = true;
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
    for (int y = 0; y < BLOCKS_DOWN; y++)
    {
        for (int x = 0; x < BLOCKS_ACROSS; x++)
        {
            struct Block* b = &game->level.blocks[x][y];

            if (b->collided)
            {
                b->inUse = false;
                b->collided = false;
                game->level.blocksLeft--;
                game->player.score += b->points;
                
                cpShapeSetLayers(b->blockShape, UNUSED_BLOCK_LAYER);
                //cpSpaceRemoveBody(game->space, b->blockBody);
                //cpSpaceRemoveShape(game->space, b->blockShape);

                cpVect pos = cpBodyGetPos(b->blockBody);
                if (randF() < POWERUP_PROB) generate_powerup(game, pos.x, pos.y);
            }
        }
    }

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

    if (game)
    {
        //TODO
    }

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

static cpBool ball_block_begin(cpArbiter *arb, cpSpace *space, void *unused)
{
    //retrieve the shapes involved
    cpShape *a, *b; 
    cpArbiterGetShapes(arb, &a, &b);

    struct Block *block = (struct Block*) cpShapeGetUserData(b);

    block->collided = true;
    //the block is attached to the section shaperp

    return true;
}

static void ball_block_post_solve(cpArbiter *arb, cpSpace *space, void *ignore)
{

}


static void post_collision_callback(cpSpace *space, cpShape *shape, void *unused)
{
    //printf("lol\n");
}

static cpBool ball_paddle_pre_solve(cpArbiter *arb, cpSpace *space, void *ignore)
{

    cpShape *a, *b; 
    cpArbiterGetShapes(arb, &a, &b);

    //struct Ball *ball = (struct Ball*) cpShapeGetUserData(a);
    //struct Paddle *paddle = (struct Paddle*) cpShapeGetUserData(b);

    cpVect vel = cpArbiterGetSurfaceVelocity(arb);

    //cpArbiterSetSurfaceVelocity(arb, cpv(500, 500));
    //cpArbiterSetElasticity(arb, 0);

    //printf("%f %f\n", vel.x, vel.y);

    cpContactPointSet set = cpArbiterGetContactPointSet(arb);
    
    for(int i=0; i<set.count; i++){
        // get and work with the collision point normal and penetration distance:
        cpVect p = set.points[i].point;             //x and y coordinate on-screen (from origin) of collision
        cpVect n = set.points[i].normal;            //the normal?
        cpFloat d = set.points[i].dist;             //the penetration distance of the collision

        //printf("(%f, %f), (%f, %f), %f\n", p.x, p.y, n.x, n.y, d);

        set.points[i].normal = cpv(0, -1);
        //set.points[i].dist = 10;
    }

    cpArbiterSetContactPointSet(arb, &set);

    return true;
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
    int paddleHeight = PADDLE_DEFAULT_HEIGHT;
    //int paddleY = 10;
    int paddleY = paddleHeight;

    //Initialise the ball
    printf("init ball start\n");
    game->ballList = NULL;
    game->ballList = g_slist_append(game->ballList, init_ball(game, 0, 0));
    printf("finished init ball\n");

    init_paddle(game);
    init_blocks(game);

    //set the collision handler for balls + block
    cpSpaceAddCollisionHandler(game->space, BALL_COLLISION_TYPE, BLOCK_COLLISION_TYPE, ball_block_begin, NULL, NULL, NULL, NULL);
    cpSpaceAddCollisionHandler(game->space, BALL_COLLISION_TYPE, PADDLE_COLLISION_TYPE, NULL, ball_paddle_pre_solve, NULL, NULL, NULL);
}

static void init_paddle(struct Game *game)
{
    //Initialise the paddle

    int paddleY = PADDLE_DEFAULT_HEIGHT;
    int paddleX = (WIDTH - PADDLE_DEFAULT_WIDTH) / 2;

    cpFloat paddleMass = 1;
    
    //make the main body. Rotational inertia of INFINITY to stop it spinning
    game->paddle.paddleBody = cpSpaceAddBody(game->space, cpBodyNew(paddleMass, INFINITY));
    
    cpBodySetVelLimit(game->paddle.paddleBody, 700);
    cpBodySetPos(game->paddle.paddleBody, cpv(paddleX + PADDLE_DEFAULT_WIDTH / 2.0, paddleY));

    //initialise the main body of the paddle 
    game->paddle.paddleShape = cpSpaceAddShape(game->space, cpBoxShapeNew(game->paddle.paddleBody, PADDLE_DEFAULT_WIDTH, PADDLE_DEFAULT_HEIGHT));
    cpShapeSetCollisionType(game->paddle.paddleShape, PADDLE_COLLISION_TYPE);
    cpShapeSetLayers(game->paddle.paddleShape, BALL_PADDLE_LAYER);
    cpShapeSetUserData(game->paddle.paddleShape, &game->paddle);

    cpShapeSetFriction(game->paddle.paddleShape, 0);
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
    cpShape* paddleDamper = cpSpaceAddShape(game->space, cpBoxShapeNew(game->paddle.paddleBody, PADDLE_DEFAULT_WIDTH, PADDLE_DEFAULT_HEIGHT));
    cpShapeSetLayers(paddleDamper, PADDLE_DAMP_LAYER);
    cpShapeSetFriction(paddleDamper, 0.9);


    game->paddleDamper = cpSegmentShapeNew(game->space->staticBody, cpv(0, paddleY - PADDLE_DEFAULT_HEIGHT + 10), cpv(WIDTH, paddleY - PADDLE_DEFAULT_HEIGHT + 10), 0);
    cpShapeSetLayers(game->paddleDamper, PADDLE_DAMP_LAYER);
    cpShapeSetFriction(game->paddleDamper, 1);

    cpSpaceAddShape(game->space, game->paddleDamper);
}

static void init_blocks(struct Game *game)
{
    for (int y = 0; y < BLOCKS_DOWN; y++)
    {
        for (int x = 0; x < BLOCKS_ACROSS; x++)
        {
            struct Block* b = &game->level.blocks[x][y];

            int blockX = x * BLOCK_WIDTH + BLOCK_OFFSET + BLOCK_WIDTH / 2;
            int blockY = HEIGHT - (y + 1) * BLOCK_HEIGHT;

            //TODO: these had 1 subtracted before, what should I do here..?
            b->width = BLOCK_WIDTH;
            b->height = BLOCK_HEIGHT;
            
            b->collided = false;

            //initialise the block body
            b->blockBody = cpSpaceAddBody(game->space, cpBodyNew(INFINITY, INFINITY));
            cpBodySetPos(b->blockBody, cpv(blockX, blockY));

            //initialise the block shape
            b->blockShape = cpSpaceAddShape(game->space, cpBoxShapeNew(b->blockBody, BLOCK_WIDTH, BLOCK_HEIGHT));
            cpShapeSetLayers(b->blockShape, BALL_PADDLE_LAYER | PADDLE_DAMP_LAYER);

            cpShapeSetFriction(b->blockShape, 0);
            cpShapeSetElasticity(b->blockShape, 1);

            //put blocks in same group, set to unused layer, set collision type, associate block to shape
            cpShapeSetGroup(b->blockShape, 2); //TODO: change this hardcoded number
            cpShapeSetLayers(b->blockShape, UNUSED_BLOCK_LAYER);
            cpShapeSetCollisionType(b->blockShape, BLOCK_COLLISION_TYPE);
            cpShapeSetUserData(b->blockShape, b);

            //setup the post-step collision callback
//            cpSpaceAddPostStepCallback(game->space, post_collision_callback, b->blockShape, NULL);
            //cpSpaceRemovePostStepCallback();
        }
    }

    printf("finished init blocks\n");
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
    game->player.lives = DEFAULT_LIVES;
    game->player.score = 0;
    game->currentLevel = 1;

    game->numBalls = 1;
    struct Ball *ball = game->ballList->data;
    ball->inUse = true;
    cpShapeSetLayers(ball->ballShape, BALL_PADDLE_LAYER | BALL_WALL_LAYER | BALL_BLOCK_LAYER);

    game->attached = true;

    reset_powerup_manager(&game->powerupManager);

    //load level 1
    populate_level(&game->level, 1);

    //add in the blocks that are current to the physics world
    for (int y = 0; y < BLOCKS_DOWN; y++)
    {
        for (int x = 0; x < BLOCKS_ACROSS; x++)
        {
            struct Block* b = &game->level.blocks[x][y];

            if (!b->inUse) continue;

            //cpShapeSetLayers(b->blockShape, BALL_BLOCK_LAYER);
            //cpSpaceAddBody(game->space, b->blockBody);
            //cpSpaceAddShape(game->space, b->blockShape);
        }
    }
}

bool is_game_over(struct Game *game)
{
    return game->player.lives == -1;
}

bool has_beaten_game(struct Game *game)
{
    return game->currentLevel == NUM_LEVELS;
}
