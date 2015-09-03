#ifndef _COLORMODIFIER_H
#define _COLORMODIFIER_H

//Stuff for the color modifiers & end point
enum colorScheme
{
	blue = 0x09,
	green = 0x0A,
	cyan = 0x0B,
	red = 0x0C,
	magenta = 0x0D,
	yellow = 0x0E,
	white = 0x0F,
	yellow2 = 0x1E
};

struct colorModifier
{
	colorScheme color;
	COORD Coord;
	char display[7];
	bool visible = true;	// check whether color ball/modifier is visible on screen
};

struct endPoint{
	COORD endPointCoord;
	bool visible = true;
	endPoint()
	{
		endPointCoord.X = 10;
		endPointCoord.Y = 1;
	}
};

void CollisionDectector();
void colorModifierCollision();
void renderColorModifier();
void renderEndPoint(endPoint& endPoint);

#endif