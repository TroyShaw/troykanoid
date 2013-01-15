#pragma once

#define MAX_SCORES 9
#define MAX_CHAR_IN_NAME 16

void loadHighscoresFromDisc();
void saveHighscoresToDisc();
void enterScore(char* name, int score);
void eraseAndCopy(char* original, char* name);

typedef struct {
    char names[MAX_SCORES][MAX_CHAR_IN_NAME];
    int scores[MAX_SCORES];
} HighscoreManager;
