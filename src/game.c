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

//Performs a reset while retaining lives, score, etc. Useful for calling between levels.
static void soft_reset_game(struct Game *game);

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

//Goes to the next level.
static void goto_next_level(struct Game *game);

//Performs a death.
static void initiate_death(struct Game *game);

//Helper method to get the first ball (assuming it's there) instead of having to go through the list functions.
static struct Ball *first_ball(struct Game *game);

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
        struct Ball *b = first_ball(game);
        cpBodyApplyImpulse(b->ballBody, cpv(450, 300), cpv(0, 0));
    } else if (!game->attached && space_pressed())
    {
        //reset_paddle(game);
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

        if (game->numBalls == 0) initiate_death(game);
        if (game->level.blocksLeft == 0) goto_next_level(game);
    }

    printf("num balls: %d\n", game->numBalls);
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
    GSList *toRemoveList = NULL;

    for (GSList *l = game->ballList; l != NULL; l = l->next)
    {
        struct Ball *ball = l->data;

        cpVect pos = cpBodyGetPos(ball->ballBody);

        if (pos.y < 0)
        {
            toRemoveList = g_slist_append(toRemoveList, ball);
        }
    }

    for (GSList *l = toRemoveList; l != NULL; l = l->next)
    {
        struct Ball *toRemove = l->data;

        game->ballList = g_slist_remove(game->ballList, toRemove);

        free_ball(game, toRemove);

        game->numBalls--;
    }

    g_slist_free(toRemoveList);
}

static void move_ball_to_player(struct Game *game)
{
    struct Ball *ball = first_ball(game);
    struct Paddle *paddle = &game->paddle;

    cpVect pos = cpBodyGetPos(paddle->paddleBody);

    cpBodySetPos(ball->ballBody, cpv(pos.x, pos.y + paddle->height / 2.0 + BALL_RADIUS));
}

static void initiate_death(struct Game *game)
{
    game->player.lives--;
    
    //if we have -1 lives, means it's gameover
    if (game->player.lives == -1) return;

    //otherwise do a soft-reset
    soft_reset_game(game);
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
    static hasInit = false;
    
    if (hasInit)
    {
        printf("Already initialised the game. Aborting.\n");
        exit(1);
    }

    hasInit = true;

    //Initialise the game space
    cpVect gravity = cpv(0, 0);
    game->space = cpSpaceNew();
    cpSpaceSetGravity(game->space, gravity);

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

    init_paddle(game);
    init_blocks(game);

    //set the collision handler for balls + block
    cpSpaceAddCollisionHandler(game->space, BALL_COLLISION_TYPE, BLOCK_COLLISION_TYPE, ball_block_begin, NULL, NULL, NULL, NULL);
    cpSpaceAddCollisionHandler(game->space, BALL_COLLISION_TYPE, PADDLE_COLLISION_TYPE, NULL, ball_paddle_pre_solve, NULL, NULL, NULL);
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
}

//Performs a reset while retaining lives, score, etc. Useful for calling between levels.
static void soft_reset_game(struct Game *game)
{
    reset_powerup_manager(&game->powerupManager);
    reset_paddle(game);

    //delete whatever balls exist and then create the initial one
    game->ballList = g_slist_append(NULL, init_ball(game, 0, 0));
    game->numBalls = 1;
    game->attached = true;
}

void reset_game(struct Game *game)
{
    soft_reset_game(game);

    game->paused = false;

    //reset the player
    game->player.lives = DEFAULT_LIVES;
    game->player.score = 0;

    //load level 1
    game->currentLevel = 1;
    populate_level(&game->level, 1);
}

void cleanup_game(struct Game *game)
{

}

bool is_game_over(struct Game *game)
{
    return game->player.lives == -1;
}

bool has_beaten_game(struct Game *game)
{
    return game->currentLevel == NUM_LEVELS;
}

static struct Ball *first_ball(struct Game *game)
{
    if (game->ballList == NULL)
    {
        printf("error: attempted to get first ball which was null\n");
        exit(1);
    }

    struct Ball *ball = game->ballList->data;
    
    return ball;
}
