#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pong.h"
#include "highscore.h"

static const char *FILE_NAME = "hs.dat";

Game game;

void loadHighscoresFromDisc()
{
    printf("loading highscore data...\n");

    FILE *fp = fopen(FILE_NAME, "r");

    if (fp == NULL)
    {
        printf("file didn't exist...\ncreating...\n");
        //file didn't exist, so load in some default data
        int i;
        for (i = 0; i < MAX_SCORES; i++)
        {
            strcpy(game.highscoreManager.names[i], "-");
            game.highscoreManager.scores[i] = 0;
        }

        //then try save it
        saveHighscoresToDisc();
        return;
    }

    //now we have to load in our saved data
    printf("file opened... loading data...\n");

    int i, j;

    for (i = 0; i < MAX_SCORES; i++)
    {
        //first load the name
        fgets(game.highscoreManager.names[i], MAX_CHAR_IN_NAME, fp);


        //then the score
        fscanf(fp, "%d\n", &game.highscoreManager.scores[i]);
    }

    //now replace \n with \0
    for (i = 0; i < MAX_SCORES; i++)
    {
        for (j = 0; j < MAX_CHAR_IN_NAME; j++)
        {
            if (game.highscoreManager.names[i][j] == '\n')
            {
                game.highscoreManager.names[i][j] = '\0';
                break;
            }
        }
    }

    int status = fclose(fp);

    if (status != 0)
    {
        printf("failed to close file when loading highscore data\n");
        exit(EXIT_FAILURE);
    }

    printf("loaded\n");
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
        fprintf(fp, game.highscoreManager.names[i]);
        fprintf(fp, "\n%d\n", game.highscoreManager.scores[i]);
    }

    int status = fclose(fp);

    if (status != 0)
    {
        printf("failed to close file when saving highscore data\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("save succesfull\n");
    }
}
void enterScore(char* name, int score)
{
    if(!name || !strlen(name))
    {
        printf("supplied name was null or empty\n");
        exit(EXIT_FAILURE);
    }
    if (score < 0)
    {
        printf("supplied score was negative\n");
        exit(EXIT_FAILURE);
    }

    int i;

    //we iterate from highest to lowest score
    //if we find a score we are bigger than,
    //we iterate from MAX_SCORE back to the score we

    for (i = 0; i < MAX_SCORES; i++)
    {
        //if the score is greater than the current score...
        if (game.highscoreManager.scores[i] == 0 || score > game.highscoreManager.scores[i])
        {
            //we want to move all old scores down, then overwrite new position
            int j;
            for (j = MAX_SCORES - 1; j > i; j--)
            {
                //move everything up 1 level
                game.highscoreManager.scores[j] = game.highscoreManager.scores[j - 1];
                strcpy(game.highscoreManager.names[j], game.highscoreManager.names[j - 1]);
            }

            //then add our new data in
            game.highscoreManager.scores[i] = score;
            strcpy(game.highscoreManager.names[i], name);

            break;
        }
    }
}

void setScore(int score)
{
    int i;
    game.highscoreManager.position = -1;

    for (i = 0; i < MAX_SCORES; i++)
    {
        if (game.highscoreManager.scores[i] == 0 || score > game.highscoreManager.scores[i])
        {
            game.highscoreManager.position = i + 1;
            return;
        }
    }
}
