#pragma once

#define CURSER_BLINK_RATE 500       //number of milliseconds between curser flashes

void glutPrint(float x, float y, char* text, float r, float g, float b, float a);
void render();
void renderGame();
void renderMenu();
