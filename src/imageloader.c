#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "imageloader.h"

#define DIR "images/"

#define PADDLE_DIR "paddle/"

//loads an image from filename and returns it as an SDL_Surface
static SDL_Surface *load_image(const char *filename);

static void load_paddle_images(void);
static void dispose_paddle_images(void);

SDL_Surface *paddleLeftBumper;
SDL_Surface *paddleRightBumper;
SDL_Surface *paddleCenter;

void load_images(void)
{
	load_paddle_images();
}

void dispose_images(void)
{
	dispose_paddle_images();
}

static void load_paddle_images()
{
	paddleLeftBumper = load_image(DIR PADDLE_DIR "paddle_left_bumper.png");
	paddleRightBumper = load_image(DIR PADDLE_DIR "paddle_left_bumper.png");
	paddleCenter = load_image(DIR PADDLE_DIR "paddle_left_bumper.png");
}

static void dispose_paddle_images()
{
	SDL_FreeSurface(paddleLeftBumper);
	SDL_FreeSurface(paddleRightBumper);
	SDL_FreeSurface(paddleCenter);
}

SDL_Surface* paddle_left_bumper_image()
{
	return paddleLeftBumper;
}

SDL_Surface* paddle_right_bumper_image()
{
	return paddleRightBumper;
}

SDL_Surface* paddle_center_image()
{
	return paddleCenter;
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
