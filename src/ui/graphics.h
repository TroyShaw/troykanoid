#pragma once

//initializes the graphics engine
void init_graphics(void);
//cleans up the graphics engine
void cleanup_graphics(void);

//fills a rectangle with the given dimensions in the last color that was set
void fill_rect(float x, float y, float w, float h);
//draws the outline of a rectangle with the given dimensions in the last color that was set
void draw_rect(float x, float y, float w, float h);
//draws a single solid line in the last color that was set
void draw_line(float x1, float y1, float x2, float y2);
//draws a circle centered at (cx, cy) of radius r, in the last color that was set
void draw_circle(float cx, float cy, float r);
//fills a circle centered at (cx, cy) of radius r, in the last color that was set
void fill_circle(float cx, float cy, float r);

//draws text centered on x axis at y value
void center_print(float y, const char* text, float r, float g, float b, float a);
//draws text at given coordinates
void draw_string(float x, float y, const char* text, float r, float g, float b, float a);

void set_color3f(int r, int g, int b);
void set_color4f(int r, int g, int b, int a);

//Returns y which considers coordinate system
float get_y(float y);
//returns the string width
int str_width(char* str);
