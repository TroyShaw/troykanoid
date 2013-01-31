#pragma once

#include <stdbool.h>

#define TICK_TIME 15
#define WIDTH 640
#define HEIGHT 480

void initGlutWindow();
void initGL();
void display();
void key(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void idle();
void process();
