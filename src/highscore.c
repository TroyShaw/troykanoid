#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highscore.h"

HighscoreManager highscoreManager;

void loadHighscoresFromDisc()
{
    //for now we'll just load in dummy data
    int i;
    for (i = 0; i < MAX_SCORES; i++)
    {
        strcpy(highscoreManager.names[i], "-");
        highscoreManager.scores[i] = 0;
    }

}
void saveHighscoresToDisc()
{

}
void enterScore(char* name, int score)
{
    if(!name || !strlen(name))
    {
        printf("supplied name was null or empty");
        exit(EXIT_FAILURE);
    }
    if (score < 0)
    {
        printf("supplied score was negative");
        exit(EXIT_FAILURE);
    }

    int i;

    for (i = 0; i < MAX_SCORES; i++)
    {
        if (score == 0 || score > highscoreManager.scores[i])
        {
            int j;
            for (j = MAX_SCORES - 1; j < i; j--)
            {
                //move everything up 1 level
                highscoreManager.scores[j] = highscoreManager.scores[j - 1];
                //highscoreManager.names[j] = highscoreManager.names[j - 1];
            }

            //then add our new data in
            highscoreManager.scores[i] = score;
            //highscoreManager.scores[i] = name;

            break;
        }
    }
}

void eraseAndCopy(char* original, char* name)
{
    while (*original)
    {

    }

}
