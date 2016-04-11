#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>

#include "texture.h"

//Initializes the window to the given length, width and title.
bool init_window(const char* title, int width, int height);

//Disposes the rendering window.
void dispose_window(void);

//Gets the SDL_Surface which represents the entire screen.
// SDL_Surface *get_screen(void);

SDL_Renderer *get_renderer(void);

//Cleans the screen to the given color.
void clear_screen(int r, int g, int b, int a);

//Applys a surface to the screen at the given coordinates.
void apply_surface(int x, int y, SDL_Surface* source);

void apply_textureReal(int x, int y, struct Texture texture);
void apply_texture(int x, int y, SDL_Texture *texture);

//Used for double buffering. Flips the buffers.
void flip_screen(void);
