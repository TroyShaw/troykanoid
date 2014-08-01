#pragma once

#define CURSER_BLINK_RATE 500       //number of milliseconds between curser flashes

#include "game.h"	
#include "highscore.h"

void init_renderer(void);
void cleanup_renderer(void);

void renderGame(struct Game *game);
void renderMenu(struct HighscoreManager *hsManager);
void renderPostGame(struct Game *game, struct HighscoreManager *hsManager);
