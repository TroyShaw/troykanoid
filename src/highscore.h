#pragma once

#define MAX_SCORES 9
#define MAX_CHAR_IN_NAME 16

//loads the highscores from the disc to the manager struct
void loadHighscoresFromDisc();
//saves the highscores in the current struct to disc
void saveHighscoresToDisc();
//enters the score and saves it in the struct
void enterScore(char* name, int score);
//sets the position variable in the struct to what position the score is
//will be between 1 and MAX_SCORES. If it is -1 it means it is did not get a placing
//doesn't actually change the highscore, just allows you to check what placing the player is
//get the players name, call enterScore(name, score), then call saveHighscoresToDisc() to save scores properly
void setScore(int score);

typedef struct {
    char names[MAX_SCORES][MAX_CHAR_IN_NAME];
    int scores[MAX_SCORES];
    int position;
    char nameBuffer[MAX_CHAR_IN_NAME];
    int bufferIndex;
} HighscoreManager;
