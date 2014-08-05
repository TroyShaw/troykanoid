##Troykanoid - an Arkanoid Clone written in C

This is a basic arkanoid clone written in C, using SDL and the Chipmunk physics engine.
<p>
It features various powerups, including lots of balls, different sized paddles, powerful comet balls, and more.<br>
It also includes a save feature which saves the best 9 scores to disc, along with the users name.
<p>
It uses the same level designs as the original arkanoid game.


###Dependencies
Troykanoid depends on a few libraries including:
- SDL 1.2
- SDL_ttf
- SDL_gfxPrimitves
- SDL_image
- Chipmunk

Ensure these libraries are present, then build by typing `make clean` followed by `make`. `make clean` is necessary first because it copies various resources files to the bin directory.

To run, execute bin/game\_debug by typing `./bin/game_debug` from the root directory of the project.
