#include "input.h"

//does a check to ensure the given keycode is within the bounds of valid keys
//stops the program with an error if it is out of bounds
static bool check_keycode(int keycode);

//gets the frame this direction was pressed.
//Uses both wasd and arrow keys.
static int frame_for_direction(Direction dir);

static bool keysHeld[MAX_KEYS] = {false};

static unsigned int keysPressedFrame[MAX_KEYS] = {0};
static unsigned int keysReleasedFrame[MAX_KEYS] = {0};

static unsigned int curKeyFrame = 1;

void keyevents_finished(void)
{
	curKeyFrame++;
}

void handle_keydown(int keycode)
{
	if (!check_keycode(keycode)) return;

	if (!keysHeld[keycode]) keysPressedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = true;
}

void handle_keyup(int keycode)
{
	if (!check_keycode(keycode)) return;

	if (keysHeld[keycode]) keysReleasedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = false;
}

bool dir_key_held(Direction direction)
{

	switch (direction)
	{
		case Up:    return keysHeld[SDLK_w];// || keysHeld[SDLK_UP]    || ;
		case Down:  return keysHeld[SDLK_s];// || keysHeld[SDLK_DOWN]  || ;
		case Left:  return keysHeld[SDLK_a];// || keysHeld[SDLK_LEFT]  || ;
		case Right: return keysHeld[SDLK_d];// || keysHeld[SDLK_RIGHT] || ;
	}

	printf("should never reach here\n");
	exit(1);
}

bool dir_pressed_now(Direction *dir)
{
	int highestPushed = 0;

	Direction dirs[4] = {Up, Left, Down, Right};

	for (int i = 3; i >= 0; i--)
	{
		if (!dir_key_held(dirs[i])) continue;

		int x = frame_for_direction(dirs[i]);

		if (x > highestPushed)
		{
			*dir = dirs[i];
			highestPushed = x;
		}
	}

	return highestPushed != 0;
}

bool key_held(int keycode)
{
	if (!check_keycode(keycode)) return false;

	return keysHeld[keycode];
}

bool key_pressed(int keycode)
{
	if (!check_keycode(keycode)) return false;

	return keysPressedFrame[keycode] == (curKeyFrame - 1);
}

bool key_released(int keycode)
{
	if (!check_keycode(keycode)) return false;

	return keysReleasedFrame[keycode] == (curKeyFrame - 1);
}

//Tests if space is pressed
bool space_pressed(void)
{
    return key_pressed(SDLK_SPACE);
}

//Tests if pause is pressed
bool pause_pressed(void)
{
	//printf("print pressed: %d\n", key_pressed(SDLK_p));
    return key_pressed(SDLK_p);
}

#define max(a, b) (a) > (b) ? (a) : (b)
#define min(a, b) max((b), (a))

static int frame_for_direction(Direction dir)
{
	switch (dir)
	{
		case Up:    return max(keysPressedFrame[SDLK_UP]   , keysPressedFrame[SDLK_w]);
		case Down:  return max(keysPressedFrame[SDLK_DOWN] , keysPressedFrame[SDLK_s]);
		case Left:  return max(keysPressedFrame[SDLK_LEFT] , keysPressedFrame[SDLK_a]);
		case Right: return max(keysPressedFrame[SDLK_RIGHT], keysPressedFrame[SDLK_d]);
	}

	printf("should never reach here\n");
	exit(1);
}

static bool check_keycode(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Keycode %d is out of range.\n", keycode);
		//printf("Aborting\n");
		//exit(1);
		return false;
	}
	else
	{
		return true;
	}
}
