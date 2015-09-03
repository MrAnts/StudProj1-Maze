#ifndef _ENEMY_H
#define _ENEMY_H

#include "Framework\timer.h"
#include "Framework\console.h"

enum myType
{
	enemy1 = 1, //enemy1 is enemy that goes Left and Right
	enemy2, //enemy2 is enemy that goes Up and down
	enemy3, //enemy3 is enemy that goes all 4 direction
	enemy4, //enemy4 is enemy that chase player
};

struct StupidComputer
{
	//Attribute of Stupid enemy
	myType type;
	COORD Coord;
	short direction, Ydirection;

	// constructor
	StupidComputer()
	{
		direction = -1; // direction -1 means going left
		type = myType::enemy1;
		colour(0x1A);	// default colour of the enemy
	}
};

void renderEnemy();

#endif