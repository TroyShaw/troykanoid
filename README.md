##Troykanoid - an Arkanoid Clone written in C

This is a basic Arkanoid clone written in C, using SDL and the Chipmunk physics engine.
<p>

![ScreenShot](http://i.imgur.com/NMN5qfH.png)

It features various powerups, including lots of balls, different sized paddles, powerful comet balls, and more.<br>
It also includes a save feature which saves the best 9 scores to disc, along with the users name.
<p>
It uses the same level designs as the original arkanoid game.

![ScreenShot](http://i.imgur.com/sL0VsXs.png)

###Dependencies
Troykanoid depends on a few libraries including:
- SDL 2
- SDL_image
- SDL_ttf
- SDL_gfxPrimitves
- SDL_mixer
- Chipmunk Physics Library

To install these libraries in Ubuntu, type

- `sudo apt-get install libsdl2-dev`
- `sudo apt-get install libsdl2-image-dev`
- `sudo apt-get install libsdl2-ttf-dev`
- `sudo apt-get install libsdl2-gfx-dev`
- `sudo apt-get install libsdl2-mixer-dev`
- `sudo apt-get install chipmunk-dev`

Ensure these libraries are present, then build by typing `make clean` followed by `make`. `make clean` is necessary first because it copies various resources files to the bin directory.

To run, execute bin/game\_debug by typing `./bin/game_debug` from the root directory of the project.
