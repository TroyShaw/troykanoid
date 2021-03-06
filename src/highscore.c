#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#include "fps.h"
#include "highscore.h"
#include "util.h"

static const char *FILE_NAME = "hs.dat";

static unsigned int lastPress;

void init_highscore_manager(struct HighscoreManager *highscoreManager)
{
    printf("loading highscore data...\n");

    FILE *fp = fopen(FILE_NAME, "r");

    if (fp == NULL)
    {
        printf("file didn't exist...\ncreating...\n");
        //file didn't exist, so load in some default data

        for (int i = 0; i < MAX_SCORES; i++)
        {
            strcpy(highscoreManager->names[i], "-");
            highscoreManager->scores[i] = 0;
        }

        //then try save it
        save_highscores(highscoreManager);
        return;
    }

    //now we have to load in our saved data
    for (int i = 0; i < MAX_SCORES; i++)
    {
        //first load the name
        fgets(highscoreManager->names[i], MAX_CHAR_IN_NAME, fp);

        //then the score
        fscanf(fp, "%d\n", &highscoreManager->scores[i]);
    }

    //now replace \n with \0
    for (int i = 0; i < MAX_SCORES; i++)
    {
        for (int j = 0; j < MAX_CHAR_IN_NAME; j++)
        {
            if (highscoreManager->names[i][j] == '\n')
            {
                highscoreManager->names[i][j] = '\0';
                break;
            }
        }
    }

    if (fclose(fp) != 0)
    {
        printf("failed to close file when loading highscore data\n");
        exit(EXIT_FAILURE);
    }
}

void save_highscores(struct HighscoreManager *highscoreManager)
{
    FILE *fp = fopen(FILE_NAME, "w");

    if (fp == NULL)
    {
        printf("failed to open file when saving highscore.\n");
        printf("highscores will persist in this game, but will be lost if the program is closed.\n");
        return;
    }
    
    for (int i = 0; i < MAX_SCORES; i++)
    {
        fprintf(fp, "%s", highscoreManager->names[i]);
        fprintf(fp, "\n%d\n", highscoreManager->scores[i]);
    }

    if (fclose(fp) != 0)
    {
        printf("failed to close file when saving highscore data.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("save successful\n");
    }
}

void enter_score(struct HighscoreManager *hm, int score)
{
    char *name = hm->nameBuffer;

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

    //we iterate from highest to lowest score
    //if we find a score we are bigger than, we iterate from MAX_SCORE back to the score
    for (int i = 0; i < MAX_SCORES; i++)
    {
        //if the score is greater than the current score...
        if (hm->scores[i] == 0 || score > hm->scores[i])
        {
            //we want to move all old scores down, then overwrite the new position
            for (int j = MAX_SCORES - 1; j > i; j--)
            {
                //move everything up 1 level
                hm->scores[j] = hm->scores[j - 1];
                strcpy(hm->names[j], hm->names[j - 1]);
            }

            //then add our new data in
            hm->scores[i] = score;
            strcpy(hm->names[i], name);

            break;
        }
    }

    //finally we reset the variables for the next game
    for (int i = 0; i < MAX_CHAR_IN_NAME; i++)
    {
        hm->nameBuffer[i] = '\0';
    }
        
    hm->bufferIndex = 0;
    hm->position = -1;
}

void set_score(struct HighscoreManager *highscoreManager, int score)
{
    highscoreManager->position = -1;

    for (int i = 0; i < MAX_SCORES; i++)
    {
        if (highscoreManager->scores[i] == 0 || score > highscoreManager->scores[i])
        {
            highscoreManager->position = i + 1;
            return;
        }
    }
}

void enter_char(struct HighscoreManager *m, unsigned char c)
{
    int i = m->bufferIndex;

    bool setTime = true;
    //ctrl + backspace / del key
    if (c == 127)
    {
        for (; i > 0; i--) m->nameBuffer[i - 1] = '\0';
        m->bufferIndex = 0;
    }
    //backspace chara
    else if (c == 8 && i != 0)
    {
        //backspace
        m->nameBuffer[i - 1] = '\0';
        m->bufferIndex--;
    }
    //only characters a-zA-Z0-9_ are valid
    else if ((isalnum(c) || isalpha(c) || c == '_'))
    {
        //restrict i so we don't go out of string
        i = min(i, MAX_CHAR_IN_NAME - 2);

        m->nameBuffer[i] = c;
        m->nameBuffer[i + 1] = '\0';
        //restrict index from going past the last null terminator
        m->bufferIndex = min(i + 1, MAX_CHAR_IN_NAME - 1);
    } 
    else 
    {
        //no valid character pressed, so don't set last-pressed time
        setTime = false; 
    }

    if (setTime)
    {
        lastPress = frames_startup();
    }
}

bool made_highscore(struct HighscoreManager *highscoreManager)
{
    return highscoreManager->position != -1;
}

bool verify_highscore_name(struct HighscoreManager *highscoreManager)
{
    return strlen(highscoreManager->nameBuffer) > 0;
}

unsigned int get_last_press(void)
{
    return frames_to_millis(frames_startup() - lastPress);
}
