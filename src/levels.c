#include "levels.h"

void populateLevel(int level)
{
    if (level < 1 || level > NUM_LEVELS)
    {
        printf("tried to load level %d", level);
        exit(1);
    }


}
