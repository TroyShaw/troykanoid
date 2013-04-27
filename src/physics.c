
#include <stdbool.h>
#include <math.h>
#include "physics.h"

#define clamp(value, min, max) ((value < min) ? min : (value > max) ? max : value)

//returns true if the point intersects with the circle
bool circleToPoint(float x1, float y1, float r, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = dx * dx + dy * dy;

    return distance <= (r * r);
}

//returns true if the circles are intersecting
bool circleToCircle(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    float distance = dx * dx + dy * dy;

    return distance <= (r1 + r2) * (r1 + r2);
}

//returns true if the rectangle intersects the point
bool rectToPoint(float x1, float y1, float w, float h, float x2, float y2)
{
    return (x2 < x1) || (x2 > (x1 + w)) || (y2 < y1) || (y2 > (y1 + h));
}

//returns true if the rectangle intersects the other rectangle
bool rectToRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    //no collision
    if (w2 <= 0 || h2 <= 0 || w1 <= 0 || h1 <= 0) return false;

    w2 += x2;
    h2 += y2;
    w1 += x1;
    h1 += y1;

    //      overflow || intersect
    return ((w2 > x1) &&
            (h2 > y1) &&
            (w1 > x2) &&
            (h1 > y2));
}

//returns true if the rectangle intersects the circle
bool rectToCircle(float x1, float y1, float w, float h, float x2, float y2, float r)
{
    float nx = clamp(x2, x1, x1 + w);
    float ny = clamp(y2, y1, y1 + h);

    float dx = x2 - nx;
    float dy = y2 - ny;

    float d = dx * dx + dy * dy;
    return d <= (r * r);
}


