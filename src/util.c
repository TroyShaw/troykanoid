#include <stdlib.h>

#include "util.h"

float randF(void)
{
    float r = (float) rand() / RAND_MAX;
    return r;
}
