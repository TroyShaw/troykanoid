#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "imageloader.h"
#include "texture.h"
#include "ui/window.h"

#define DIR "images/"

#define PADDLE_DIR "paddle/"
#define BRICKS_DIR "bricks/"
#define POWERUP_DIR "powerups/"

//loads an image from filename and returns it as a Texture
static struct Texture load_image(const char *filename);

static void load_paddle_images(void);
static void dispose_paddle_images(void);
static void load_powerup_images(void);
static void dispose_powerup_images(void);
static void load_brick_images(void);
static void dispose_brick_images(void);

static struct Texture paddleLeftBumper;
static struct Texture paddleRightBumper;
static struct Texture paddleCenter;

static struct Texture powerup;

static struct Texture ball;

static struct Texture background;

static struct Texture whiteBrick, orangeBrick, cyanBrick, greenBrick, redBrick;
static struct Texture blueBrick, pinkBrick, yellowBrick, silverBrick, goldBrick;

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
    // SDL_FreeSurface(paddleLeftBumper);
    // SDL_FreeSurface(paddleRightBumper);
    // SDL_FreeSurface(paddleCenter);
}

static void load_powerup_images(void)
{
    powerup = load_image(DIR POWERUP_DIR "powerup.png");
}

static void dispose_powerup_images(void)
{
    // SDL_FreeSurface(powerup);
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
    // SDL_FreeSurface(whiteBrick);
    // SDL_FreeSurface(orangeBrick);
    // SDL_FreeSurface(cyanBrick);
    // SDL_FreeSurface(greenBrick);
    // SDL_FreeSurface(redBrick);
    // SDL_FreeSurface(blueBrick);
    // SDL_FreeSurface(pinkBrick);
    // SDL_FreeSurface(yellowBrick);
    // SDL_FreeSurface(silverBrick);
    // SDL_FreeSurface(goldBrick);
}

struct Texture paddle_left_bumper_image(void)
{
    return paddleLeftBumper;
}

struct Texture paddle_right_bumper_image(void)
{
    return paddleRightBumper;
}

struct Texture paddle_center_image()
{
    return paddleCenter;
}

struct Texture ball_image(void)
{
    return ball;
}

struct Texture powerup_image(void)
{
    return powerup;
}

struct Texture background_image(void)
{
    return background;
}

struct Texture brick_white_image(void)
{
    return whiteBrick;
}

struct Texture brick_orange_image(void)
{
    return orangeBrick;
}

struct Texture brick_cyan_image(void)
{
    return cyanBrick;
}

struct Texture brick_green_image(void)
{
    return greenBrick;
}

struct Texture brick_red_image(void)
{
    return redBrick;
}

struct Texture brick_blue_image(void)
{
    return blueBrick;
}

struct Texture brick_pink_image(void)
{
    return pinkBrick;
}

struct Texture brick_yellow_image(void)
{
    return yellowBrick;
}

struct Texture brick_silver_image(void)
{
    return silverBrick;
}

struct Texture brick_gold_image(void)
{
    return goldBrick;
}


static struct Texture load_image(const char *filename)
{
    printf("loading image: %s\n", filename);
    
    //The image that's loaded
    SDL_Surface* loadedImage = IMG_Load(filename);

    SDL_SetColorKey( loadedImage, SDL_TRUE, SDL_MapRGB( loadedImage->format, 0, 0xFF, 0xFF ) );
    SDL_Texture *newTexture = SDL_CreateTextureFromSurface( get_renderer(), loadedImage );

    SDL_FreeSurface(loadedImage);

    struct Texture texture;

    texture.texture = newTexture;
    texture.w = loadedImage->w;
    texture.h = loadedImage->h;

    return texture;
    //If the image loaded
    // if(loadedImage != NULL)
    // {
    //     //Create an optimized image
    //     optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

    //     //Free the old image
    //     SDL_FreeSurface(loadedImage);
    // }
    // else
    // {
    //     printf("Error opening image %s\n", filename);
    //     printf("Aborting.\n");
    //     exit(1);
    // }

    //Return the optimized image
    // return optimizedImage;
}
