#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "texture.h"

void load_images(void);
void dispose_images(void);

struct Texture paddle_left_bumper_image(void);
struct Texture paddle_right_bumper_image(void);
struct Texture paddle_center_image(void);

struct Texture ball_image(void);

struct Texture powerup_image(void);

struct Texture bullet_image(void);

struct Texture background_image(void);

struct Texture brick_white_image(void);
struct Texture brick_orange_image(void);
struct Texture brick_cyan_image(void);
struct Texture brick_green_image(void);
struct Texture brick_red_image(void);
struct Texture brick_blue_image(void);
struct Texture brick_pink_image(void);
struct Texture brick_yellow_image(void);
struct Texture brick_silver_image(void);
struct Texture brick_gold_image(void);
