#pragma once

//Defines the four cardinal directions.
typedef enum
{
	Up,
	Down,
	Left,
	Right
} Direction;

//Defines the four ordinal directions.
typedef enum
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight
} DiagDirection;

//Copies -1, 0, 1 to the given x and y ints depending on what direction is given.
void dir_xy(Direction dir, int *x, int *y);

//Returns the opposite direction to the one given.
Direction dir_opposite(Direction dir);
