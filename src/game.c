#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "highscore.h"
#include "input.h"
#include "main.h"
#include "physics.h"

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
    //do this here so if they pushed space this tick, they get processed immediately
    if (game->attached) game->attached = !space_pressed();

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

    if (dir_key_held(Left))  x -= 1;
    if (dir_key_held(Right)) x += 1;

    //move paddle
    paddle->x += x * PLAYER_MOVE_SPEED;

    //if paddle out of bounds of screen, move them within
    paddle->x = min(max(paddle->x, 0), WIDTH - paddle->width);
}

static void move_balls(struct Game *game)
{
    struct Ball *ball = NULL;

    for (int i = 0; i < game->numBalls; i++)
    {
//if we move a ball, we redo the loop using the same i so that it also gets counted
redo:
        ball = &game->balls[i];

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

        if (game->powerupManager.forceField && ball->y < 10)
        {
            ball->y = 10;
            ball->velY *= -1;
            game->powerupManager.forceField = false;
        }

        if (ball->y < 0)
        {
            game->balls[i] = game->balls[max(game->numBalls - 1, 0)];
            game->balls[max(game->numBalls - 1, 0)].inUse = false;
            //ball->inUse = false;
            game->numBalls--;

            //if there are no balls left, no point iterating over the rest
            //also if i == game->numBalls otherwise we'll iterate over the last (and dead) ball
            if (game->numBalls == 0 || i == game->numBalls) return;
            //otherwise we have to redo the loop with this new ball so it's processed
            else goto redo;
        }
        else if (ball->y + 2 * ball->radius > HEIGHT)
        {
            ball->y = HEIGHT - 2 * ball->radius;
            ball->velY *= -1;
        }
    }
}

static void move_ball_to_player(struct Game *game)
{
    struct Ball *ball = &game->balls[0];
    struct Paddle *paddle = &game->paddle;

    ball->y = paddle->y + paddle->height;
    ball->x = paddle->x + paddle->width / 2 - ball->radius;
}

static void initiate_death(struct Game *game)
{
    //subtract a life
    game->player.lives--;
    
    //if we have -1 lives, means it's gameover
    if (game->player.lives == -1) return;

    //reset paddle size and position
    game->paddle.realWidth = PADDLE_DEFAULT_WIDTH;
    game->paddle.width = PADDLE_DEFAULT_WIDTH;
    game->paddle.x = (WIDTH - game->paddle.width) / 2;

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
    //algorithm:
    //  we have a 3x3 2d array of bools that represent collisions with adjacent squares
    //  imagine the ball is in the center of the array (1, 1) and each
    int x, y, xV, yV;
    struct Block *bl = NULL;
    struct Ball *b = NULL;
    
    //true if the ball collided in either direction this tick
    //invert that directions velocity after iterating over all balls to change direction only once
    bool collided[3][3];

    for (int i = 0; i < game->numBalls; i++)
    {
        b = &game->balls[i];
        //reset collision array
        for (int j = 0; j < 9; j++)
        {
            collided[j % 3][j / 3] = false;
        }

        //get (x,y) of ball on screen in blocks. Coordinate system starts (0,0) at top left corner
        x = (float) (b->x + b->radius) / WIDTH * BLOCKS_ACROSS;
        y = (HEIGHT - (b->y + b->radius)) / game->level.blocks[0]->height;

        //we default to nocd t affecting velocities
        xV = 0;
        yV = 0;

        for (int j = -1; j < 2; j++)
        {
            for (int k = -1; k < 2; k++)
            {
                int ix = j + x;
                int iy = k + y;
                
                if (ix < 0 || iy < 0 || ix >= BLOCKS_ACROSS || iy >= BLOCKS_DOWN) continue;

                bl = &game->level.blocks[ix][iy];

                bool isCollided = rectToRect(bl->x, bl->y, bl->width, bl->height, b->x, b->y, b->radius * 2, b->radius * 2);
                collided[j + 1][k + 1] = bl->inUse && isCollided;
            }
        }

        for (int j = 0; j < 9; j++) if (collided[j % 3][j / 3]) goto skip;
        goto lol;
        skip:

        printf("%d%d%d\n", collided[0][0], collided[1][0], collided[2][0]);
        printf("%d%d%d\n", collided[0][1], collided[1][1], collided[2][1]);
        printf("%d%d%d\n\n", collided[0][2], collided[1][2], collided[2][1]);

        lol:

        //diagonals
        if (collided[0][1] && collided[1][0])
        {
            collided[0][0] = false;
            xV = 1;
            yV = -1;
        }
        else if (collided[1][0] && collided[2][1])
        {
            collided[2][0] = false;
            xV = 1;
            yV = -1;
        }
        else if (collided[0][1] && collided[1][2])
        {
            collided[0][2] = false;
            xV = 1;
            yV = 1;
        }
        else if (collided[1][2] && collided[2][1])
        {
            collided[2][2] = false;
            xV = -1;
            yV = 1;
        }

        //left and right
        if ((collided[0][0] && collided[0][1]) || (collided[0][1] && collided[0][2]))
        {
            xV = 1;
            yV = 0;
        }
        else if ((collided[2][0] && collided[2][1]) || (collided[2][1] && collided[2][2]))
        {
            xV = -1;
            yV = 0;
        }

        //top and bottom
        if ((collided[0][0] && collided[1][0]) || (collided[1][0] && collided[2][0]))
        {
            xV = 0;
            yV = -1;
        }
        else if ((collided[0][2] && collided[1][2]) || (collided[1][2] && collided[2][2]))
        {
            xV = 0;
            yV = 1;
        }

        //single middles
        if (collided[1][0])
        {
            xV = 0;
            yV = -1;
        }
        if (collided[0][1])
        {
            xV = 1;
            yV = 0;
        }
        if (collided[1][2])
        {
            xV = 0;
            yV = 1;
        }
        if (collided[2][1])
        {
            xV = -1;
            yV = 0;
        }

        //adjust velocities based on collisions
        if (xV != 0) b->velX = xV * abs(b->velX);
        if (yV != 0) b->velY = yV * abs(b->velY);

        //destroy blocks now
        for (int j = -1; j < 2; j++)
        {
            for (int k = -1; k < 2; k++)
            {
                int ix = j + x;
                int iy = k + y;
                
                if (!collided[j + 1][k + 1] || ix < 0 || iy < 0 || ix >= BLOCKS_ACROSS || iy >= BLOCKS_DOWN) continue;

                bl = &game->level.blocks[ix][iy];

                if (!bl->indestructable)
                {
                    //ball wasn't indestructable, so set it to not used, subtract from total blocks, increment score
                    bl->inUse = false;
                    game->level.blocksLeft--;
                    game->player.score += bl->points;
                    if (randF() < POWERUP_PROB) generatePowerup(game, bl->x + bl->width / 2, bl->y + bl->height);
                }
            }
        }
    }
}

static void player_ball_collision(struct Game *game)
{
    struct Paddle *paddle = &game->paddle;

    for (int i = 0; i < game->numBalls; i++)
    {
        struct Ball *ball = &game->balls[i];

        if (rectToRect(paddle->x, paddle->y, paddle->width, paddle->height, ball->x, ball->y, ball->radius * 2, ball->radius * 2))
        {
            float pos = (ball->x + ball->radius - paddle->x) / (float) paddle->width;
            
            if (pos < 0) pos = 0;
            if (pos > 1) pos = 1;

            ball->velY *= -1;
            ball->velX = 5.0f * (2.0f * pos - 1.0f);
            ball->y = paddle->y + paddle->height;
        }
    }
}

void init_game(struct Game *game)
{
    game->paused = false;

    game->paddle.color = (SDL_Color) {128, 128, 128, 128};
    game->paddle.width = PADDLE_DEFAULT_WIDTH;
    game->paddle.realWidth = game->paddle.width;
    game->paddle.x = (WIDTH - game->paddle.width) / 2;
    game->paddle.y = 10;
    game->paddle.height = 15;
    game->player.lives = DEFAULT_LIVES;
    game->player.score = 0;
    game->currentLevel = 1;

    for (int i = 0; i < BALL_ARRAY_SIZE; i++)
    {
        struct Ball *ball = &game->balls[i];

        ball->color = (SDL_Color) {255, 179, 179, 179};
        ball->radius = 7;
        ball->x = (WIDTH - ball->radius * 2) / 2;
        ball->y = game->paddle.y + game->paddle.height;
        ball->velX = 5;
        ball->velY = 5;
        ball->inUse = false;
    }

    game->numBalls = 1;
    game->balls[0].inUse = true;

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
