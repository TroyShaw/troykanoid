
#include "game_entities.h"

struct Ball *init_ball(struct Game *game, float x, float y)
{
    struct Ball *ball = malloc(sizeof *ball);

    ball->inUse = false;

    cpFloat mass = 1;
    cpFloat moment = cpMomentForCircle(mass, 0, BALL_RADIUS, cpvzero);
    ball->ballBody = cpSpaceAddBody(game->space, cpBodyNew(mass, moment));

    cpBodySetPos(ball->ballBody, cpv(x, y));
    
    ball->ballShape = cpSpaceAddShape(game->space, cpCircleShapeNew(ball->ballBody, BALL_RADIUS, cpvzero));
    cpShapeSetFriction(ball->ballShape, 0);
    cpShapeSetElasticity(ball->ballShape, 1);

    //set balls to same group
    //TODO: make this number something else
    cpShapeSetGroup(ball->ballShape, 1);
    cpShapeSetCollisionType(ball->ballShape, BALL_COLLISION_TYPE);
    cpShapeSetUserData(ball->ballShape, ball);

    ball->inUse = true;
    cpShapeSetLayers(ball->ballShape, BALL_PADDLE_LAYER | BALL_WALL_LAYER | BALL_BLOCK_LAYER);

    return ball;
}
