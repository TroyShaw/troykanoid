#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "imageloader.h"

#define DIR "images/"

#define PADDLE_DIR "paddle/"
#define BRICKS_DIR "bricks/"
#define POWERUP_DIR "powerups/"

//loads an image from filename and returns it as an SDL_Surface
static SDL_Surface *load_image(const char *filename);

static void load_paddle_images(void);
static void dispose_paddle_images(void);
static void load_powerup_images(void);
static void dispose_powerup_images(void);
static void load_brick_images(void);
static void dispose_brick_images(void);

static SDL_Surface *paddleLeftBumper;
static SDL_Surface *paddleRightBumper;
static SDL_Surface *paddleCenter;

static SDL_Surface *powerup;

static SDL_Surface *ball;

static SDL_Surface *background;

static SDL_Surface *whiteBrick, *orangeBrick, *cyanBrick, *greenBrick, *redBrick;
static SDL_Surface *blueBrick, *pinkBrick, *yellowBrick, *silverBrick, *goldBrick;

void load_images(void)
{
    load_powerup_images();
    load_paddle_images();
    load_brick_images();

    ball = load_image(DIR "ball.png");

    background = load_image(DIR "backgrounds/galaxy/galaxy-X.tga");
}

void dispose_images(void)
{
    dispose_powerup_images();
    dispose_paddle_images();
    dispose_brick_images();
}

static void load_paddle_images(void)
{
    paddleLeftBumper = load_image(DIR PADDLE_DIR "paddle_left_bumper.png");
    paddleRightBumper = load_image(DIR PADDLE_DIR "paddle_right_bumper.png");
    paddleCenter = load_image(DIR PADDLE_DIR "paddle_center.png");
}

static void dispose_paddle_images(void)
{
    SDL_FreeSurface(paddleLeftBumper);
    SDL_FreeSurface(paddleRightBumper);
    SDL_FreeSurface(paddleCenter);
}

static void load_powerup_images(void)
{
    powerup = load_image(DIR POWERUP_DIR "powerup.png");
}

static void dispose_powerup_images(void)
{
    SDL_FreeSurface(powerup);
}

static void load_brick_images(void)
{
    whiteBrick  = load_image(DIR BRICKS_DIR "brick_white.png");
    orangeBrick = load_image(DIR BRICKS_DIR "brick_orange.png");
    cyanBrick   = load_image(DIR BRICKS_DIR "brick_cyan.png");
    greenBrick  = load_image(DIR BRICKS_DIR "brick_green.png");
    redBrick    = load_image(DIR BRICKS_DIR "brick_red.png");
    blueBrick   = load_image(DIR BRICKS_DIR "brick_blue.png");
    pinkBrick   = load_image(DIR BRICKS_DIR "brick_pink.png");
    yellowBrick = load_image(DIR BRICKS_DIR "brick_yellow.png");
    silverBrick = load_image(DIR BRICKS_DIR "brick_silver.png");
    goldBrick   = load_image(DIR BRICKS_DIR "brick_gold.png");
}

static void dispose_brick_images(void)
{
    SDL_FreeSurface(whiteBrick);
    SDL_FreeSurface(orangeBrick);
    SDL_FreeSurface(cyanBrick);
    SDL_FreeSurface(greenBrick);
    SDL_FreeSurface(redBrick);
    SDL_FreeSurface(blueBrick);
    SDL_FreeSurface(pinkBrick);
    SDL_FreeSurface(yellowBrick);
    SDL_FreeSurface(silverBrick);
    SDL_FreeSurface(goldBrick);
}

SDL_Surface* paddle_left_bumper_image(void)
{
    return paddleLeftBumper;
}

SDL_Surface* paddle_right_bumper_image(void)
{
    return paddleRightBumper;
}

SDL_Surface* paddle_center_image()
{
    return paddleCenter;
}

SDL_Surface* ball_image(void)
{
    return ball;
}

SDL_Surface* powerup_image(void)
{
    return powerup;
}

SDL_Surface* background_image(void)
{
    return background;
}

SDL_Surface* brick_white_image(void)
{
    return whiteBrick;
}

SDL_Surface* brick_orange_image(void)
{
    return orangeBrick;
}

SDL_Surface* brick_cyan_image(void)
{
    return cyanBrick;
}

SDL_Surface* brick_green_image(void)
{
    return greenBrick;
}

SDL_Surface* brick_red_image(void)
{
    return redBrick;
}

SDL_Surface* brick_blue_image(void)
{
    return blueBrick;
}

SDL_Surface* brick_pink_image(void)
{
    return pinkBrick;
}

SDL_Surface* brick_yellow_image(void)
{
    return yellowBrick;
}

SDL_Surface* brick_silver_image(void)
{
    return silverBrick;
}

SDL_Surface* brick_gold_image(void)
{
    return goldBrick;
}


static SDL_Surface *load_image(const char *filename)
{
    printf("loading image: %s\n", filename);
    
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image using SDL_image
    loadedImage = IMG_Load(filename);

    //If the image loaded
    if(loadedImage != NULL)
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

        //Free the old image
        SDL_FreeSurface(loadedImage);
    }
    else
    {
        printf("Error opening image %s\n", filename);
        printf("Aborting.\n");
        exit(1);
    }

    //Return the optimized image
    return optimizedImage;
}
