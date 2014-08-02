#pragma once

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))

#define clamp(value, min, max) (((value) < (min)) ? (min) : ((value) > (max)) ? (max) : (value))

//returns a random float between 0-1
float randF(void);

