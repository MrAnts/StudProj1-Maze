#ifndef _GAME_H
#define _GAME_H

#include "Framework\timer.h"
#include "Enemy.h"
#include "ColorModifier.h"
#include "Menu.h"
#include <string>

extern CStopWatch g_swTimer;
extern bool g_bQuitGame;
static int choice = 1;
static int optchoose = 1;
extern bool g_menu;

// Enumeration to store the control keys that your game will have
enum EKEYS
{
	K_UP,
	K_DOWN,
	K_LEFT,
	K_RIGHT,
	K_ESCAPE,
	K_SPACE,
	K_PAUSE,
	K_ENTER,
	K_COUNT
};

// Enumeration for the different screen states
enum EGAMESTATES
{
	S_OPTIONS,
	S_INSTRUCTIONS,
	S_SPLASHSCREEN,
	S_GAME,
	S_WIN,
	S_LOSE,
	S_HIGHSCORE,
	S_COUNT
};

// struct for the game character
struct SGameChar
{
	COORD m_cLocation;
	bool m_bActive;
	bool m_bActive2;
};

struct Description
{
	colorScheme color;
	COORD Coord;
	Description()
	{
		color = yellow2;
	}
};

struct Player
{
	myType type;
	int hp = 3;
	COORD Coord;

	// constructor
	Player()
	{
		Coord.X = 1;
		Coord.Y = 1;
	}
};



void init        ( void );      // initialize your variables, allocate memory, etc
void getInput    ( void );      // get input from player
void update      ( double dt ); // update the game and the state of the game
void render      ( void );      // renders the current state of the game to the console
void shutdown    ( void );      // do clean up, free memory

void splashScreenWait();    // waits for time to pass in splash screen
void gameplay();            // gameplay logic
void moveCharacter();       // moves the character, collision detection, physics, etc
void processUserInput();    // checks if you should change states or do something else with the game, e.g. pause, exit
void clearScreen();         // clears the current screen and draw from scratch 
void renderSplashScreen();  // renders the splash screen
void renderGame();          // renders the game stuff
void renderMap();           // renders the map to the buffer first
void renderCharacter();     // renders the character into the buffer
void renderFramerate();     // renders debug information, frame rate, elapsed time, etc
void renderToScreen();      // dump the contents of the buffer to the screen, one frame worth of game


void StoreCoord(StupidComputer& stupidObj, short CoordX, short CoordY);
void StoreCoord(colorModifier& stupidObj, short CoordX, short CoordY);
void StoreCoord(Description& stupidObj, short CoordX, short CoordY);


struct g_Object      //Struct g_Object is like a container storing all the other game object in the game
{
	//Storage_SC 0 is for Enemy that only walk left right, 
	//Storage_SC 1 is for Enemy that only walk Up down,
	//Storage_SC 2 is for Enemy that can walk all 4 direction.
	//Storage_SC 3 is for Enemy that chase after player.

	StupidComputer Storage_SC[20]; //Total Numbers of AI in the game
	StupidComputer Storage_CAI[6];
	colorModifier Storage_CM[10]; //Total Numbers of ColorModifier in the game

	myType EnemyType = myType::enemy1; // 1st enemy type (enemy1) = enemy that move left and right only
	g_Object();

	
};// this is where the main thing


void AIMovementLR(g_Object& ai);
void AIMovementUD(g_Object& ai);
void AIMovementUDLR(g_Object& ai);
void AIMovementHoming(g_Object& ai);

#endif // _GAME_H