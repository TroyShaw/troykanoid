#pragma once

#define MAX_SCORES 9
#define MAX_CHAR_IN_NAME 14

//loads the highscores from the disc to the manager struct
void loadHighscoresFromDisc();
//saves the highscores in the current struct to disc
void saveHighscoresToDisc();
//enters the score and saves it in the struct
void enterScore(int score);
//sets the position variable in the struct to what position the score is
//will be between 1 and MAX_SCORES. If it is -1 it means it is did not get a placing
//doesn't actually change the highscore, just allows you to check what placing the player is.
//Call enterScore(score), then call saveHighscoresToDisc() to save scores properly
void setScore(int score);
//enters the given chara into the character buffer
//invalid characters are ignored
//valid characters are entered into the end of the buffer until there is no room left
//characters entered after this replace the last character
//backspace removes the last character, or is ignored if no character present
void enterChar(unsigned char c);
//verifies the highscore name
//this checks it is of valid length, has no invalid charas, etc
bool verifyHighscoreName();

typedef struct {
    char names[MAX_SCORES][MAX_CHAR_IN_NAME];
    int scores[MAX_SCORES];
    int position;
    char nameBuffer[MAX_CHAR_IN_NAME];
    int bufferIndex;
} HighscoreManager;
