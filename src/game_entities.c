
#include "defines.h"
#include "game_entities.h"

struct Ball *init_ball(struct Game *game, float x, float y)
{
    struct Ball *ball = malloc(sizeof *ball);

    cpFloat mass = 1;
    cpFloat moment = cpMomentForCircle(mass, 0, BALL_RADIUS, cpvzero);
    ball->ballBody = cpSpaceAddBody(game->space, cpBodyNew(mass, moment));

    cpBodySetPos(ball->ballBody, cpv(x, y));
    
    ball->ballShape = cpSpaceAddShape(game->space, cpCircleShapeNew(ball->ballBody, BALL_RADIUS, cpvzero));
    cpShapeSetElasticity(ball->ballShape, 1);

    //set balls to same group
    //TODO: make this number something else
    cpShapeSetGroup(ball->ballShape, 1);
    cpShapeSetCollisionType(ball->ballShape, BALL_COLLISION_TYPE);
    cpShapeSetUserData(ball->ballShape, ball);

    cpShapeSetLayers(ball->ballShape, BALL_PADDLE_LAYER | BALL_WALL_LAYER | BALL_BLOCK_LAYER);

    return ball;
}

void free_ball(struct Game *game, struct Ball *ball)
{
    cpSpaceRemoveBody(game->space, ball->ballBody);
    cpSpaceRemoveShape(game->space, ball->ballShape);

    cpBodyFree(ball->ballBody);
    cpShapeFree(ball->ballShape);

    free(ball);
}

void init_paddle(struct Game *game)
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

//Resets a paddle back to its starting location and size.
void reset_paddle(struct Game *game)
{
    //reset the width, the location, and also any movement speed
    int paddleY = PADDLE_DEFAULT_HEIGHT;
    int paddleX = (WIDTH - PADDLE_DEFAULT_WIDTH) / 2;

    cpVect pos = cpBodyGetPos(game->paddle.paddleBody);

    cpBodySetPos(game->paddle.paddleBody, cpv(paddleX + PADDLE_DEFAULT_WIDTH / 2.0, pos.y));
    cpBodyResetForces(game->paddle.paddleBody);
    cpBodySetVel(game->paddle.paddleBody, cpv(0, 0));

    game->paddle.width = PADDLE_DEFAULT_WIDTH;
    game->paddle.realWidth = PADDLE_DEFAULT_WIDTH;
    game->paddle.height = PADDLE_DEFAULT_HEIGHT;
}
