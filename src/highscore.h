#pragma once

#define MAX_SCORES 9
#define MAX_CHAR_IN_NAME 14

//Holds all data related to high-scores in the game.
struct HighscoreManager {
    char names[MAX_SCORES][MAX_CHAR_IN_NAME];
    int scores[MAX_SCORES];
    int position;
    char nameBuffer[MAX_CHAR_IN_NAME];
    int bufferIndex;
};

//Initializes the high-score manager.
//Attempts to load the high-scores from the disc to the manager struct.
//If the high-score file doesn't exist, it creates it with initial names "-" and scores 0.
void init_highscore_manager(struct HighscoreManager *highscoreManager);

//Saves the high-scores saved to the high-score manager to disk.
void save_highscores(struct HighscoreManager *highscoreManager);

//Enters the score and saves it in the struct.
void enter_score(struct HighscoreManager *highscoreManager, int score);

//Sets the position variable in the struct to what position the score is
//will be between 1 and MAX_SCORES. If it is -1 it means it is did not get a placing.
//This function doesn't actually change the high-score, it just allows you to check what placing the player is.
//Call enterScore(score), then call saveHighscoresToDisc() to save scores properly.
void set_score(struct HighscoreManager *highscoreManager, int score);

//Enters the given char into the character buffer.
//Invalid characters are ignored.
//Valid characters are entered into the end of the buffer until there is no room left.
//Characters entered after this replace the last character.
//Backspace removes the last character, or is ignored if no character present.
void enter_char(struct HighscoreManager *highscoreManager, unsigned char c);

//Verifies the high-score name has more than 1 character.
bool verify_highscore_name(struct HighscoreManager *highscoreManager);

//Returns true if the user made it into the highscore table with the last entered score.
bool made_highscore(struct HighscoreManager *highscoreManager);

//Returns the time in milliseconds since the last valid key press (backspace included).
unsigned int get_last_press(void);
