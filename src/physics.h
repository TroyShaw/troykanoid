#pragma once

#include <stdbool.h>

//returns true if the circles are intersecting
bool circleToCircle(float x1, float y1, float r1, float x2, float y2, float r2);
//returns true if the point intersects with the circle
bool circleToPoint(float x1, float y1, float r, float x2, float y2);
//returns true if the rectangle intersects the other rectangle
bool rectToRect(float x1, float y1, float l1, float h1, float x2, float y2, float l2, float h2);
//returns true if the rectangle intersects the circle
bool rectToCircle(float x1, float y1, float l1, float h1, float x2, float y2, float r);
//returns true if the rectangle intersects the point
bool rectToPoint(float x1, float y1, float l1, float h1, float x2, float y2);
