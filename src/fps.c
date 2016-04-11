#include "fps.h"

#include <SDL2/SDL2_framerate.h>

static FPSmanager fpsManager;
static int fpsHz;
static int numTicks;

void fps_init(int hz)
{
	fpsHz = hz;
	SDL_initFramerate(&fpsManager);
	fps_sethz(hz);
}

void fps_sethz(int hz)
{
	fpsHz = hz;
	SDL_setFramerate(&fpsManager, hz);
}

void fps_sleep(void)
{
	numTicks++;
	SDL_framerateDelay(&fpsManager);
}

unsigned int ticks_game(void)
{
	return SDL_GetTicks();
}

unsigned int ticks_startup()
{
	return SDL_GetTicks();
}

unsigned int frames_game(void)
{
	return numTicks;
}

unsigned int frames_startup(void)
{
	return numTicks;
}

unsigned int frames_to_millis(unsigned int frames)
{
	return 1000.0f / fpsHz * frames;
}
