#pragma once

#define CURSER_BLINK_RATE 500       //number of milliseconds between curser flashes

void init_renderer(void);

void render(struct Game *game);
void renderGame(struct Game *game);
void renderMenu(struct Game *game);

void draw_string(float x, float y, char* text, float r, float g, float b, float a);
