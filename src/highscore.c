#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highscore.h"

HighscoreManager highscoreManager;


static const char *FILE_NAME = "hs.dat";

void loadHighscoresFromDisc()
{
    FILE *fp = fopen(FILE_NAME, "r");

    if (fp == NULL)
    {
        //file didn't exist, so load in some default data
        int i;
        for (i = 0; i < MAX_SCORES; i++)
        {
            strcpy(highscoreManager.names[i], "-");
            highscoreManager.scores[i] = 0;
        }

        //then try save it
        printf("ad");
        saveHighscoresToDisc();
        return;
    }

    //now we have to load in our saved data

    int status = fclose(fp);

    if (status != 0)
    {
        printf("failed to close file when loading highscore data\n");
        exit(EXIT_FAILURE);
    }
}
void saveHighscoresToDisc()
{
    FILE *fp = fopen(FILE_NAME, "w");

    if (fp == NULL)
    {
        printf("failed to open file when saving highscore.\n");
        printf("highscores will persist in this game, but will be lost if the program is closed.\n");
        return;
    }

    int i;
    for (i = 0; i < MAX_SCORES; i++)
    {
        fprintf(fp, highscoreManager.names[i]);
        fprintf(fp, "\n%d\n", highscoreManager.scores[i]);
    }

    int status = fclose(fp);

    if (status != 0)
    {
        printf("failed to close file when saving highscore data\n");
        exit(EXIT_FAILURE);
    }
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
