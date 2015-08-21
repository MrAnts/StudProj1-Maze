// This is the main file for the game logic and function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>

double  g_dElapsedTime;
double  g_dDeltaTime;
bool    g_abKeyPressed[K_COUNT];

// Game specific variables here
SGameChar   g_sChar;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once

// Console object
Console g_Console(80, 30, "RGB Maze");

const int row = 20;		//<----------The MAP Length and Height
const int col = 60;
int maze[row][col] = {
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 1 },
	{ 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1 },
	{ 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
	{ 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
	{ 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 },
	{ 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1 },
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1 },
	{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};


//Array for all the coordinates for Stupid Computer ai
short SC_CoordX[3] = {60,45,50};
short SC_CoordY[3] = {1,15,20};

//Win and Lose Bool
bool win = false;

enum myType
{
	boundry,
	enemy1, //enemy1 is enemy that goes Left and Right
	enemy2, //enemy2 is enemy that goes Up and down
	enemy3, //enemy3 is enemy that goes all 4 direction
	colorball,
};


struct StupidComputer
{
	myType type;
	COORD Coord;
	short direction;

	// constructor
	StupidComputer()
	{
		direction = -1;
		type = enemy1;
		colour(0x1A);
	}
};
struct ColorModifier
{
	myType type;
	COORD Coord;
	bool visible = true;
	ColorModifier()
	{
		type = colorball;
		Coord.X = 28;
		Coord.Y = 10;
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

void StoreCoord(StupidComputer& stupidObj, short CoordX, short CoordY)
{
	stupidObj.Coord.X = CoordX;
	stupidObj.Coord.Y = CoordY;
}
void StoreCoord(ColorModifier& colorObj, short CoordX, short CoordY)
{
	colorObj.Coord.X = CoordX;
	colorObj.Coord.Y = CoordY;
}

struct g_Object
{
	//Storage_SC 0 is for Enemy that only walk left right, 
	//Storage_SC 1 is for Enemy that only walk Up down,
	//Storage_SC 2 is for Enemy that can walk all 4 direction.

	StupidComputer Storage_SC[3];
	ColorModifier Storage_CM;
	Player player1;
	myType EnemyType = enemy1;

	g_Object()
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			StoreCoord(Storage_SC[i], SC_CoordX[i], SC_CoordY[i]);
			EnemyType = static_cast<myType>(enemy1 + i);
			Storage_SC[i].type = EnemyType;
		}
		StoreCoord(Storage_CM, 28, 10);
	}
};


g_Object Stage1;
ColorModifier color1;
Player player1;

bool goingRight = false, goingLeft = true, goingUp = false, goingDown = false;
bool colorChanged = false;
//HengSoon Code End
//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------

void init( void )
{

    // Set precision for floating point output
    g_dElapsedTime = 0.0;
    g_dBounceTime = 0.0;

    // sets the initial state for the game
    g_eGameState = S_SPLASHSCREEN;

    g_sChar.m_cLocation.X = 1;
    g_sChar.m_cLocation.Y = 1;
    g_sChar.m_bActive = true;

    // sets the width, height and the font name to use in the console
    g_Console.setConsoleFont(0, 16, L"Consolas");
}

//--------------------------------------------------------------
// Purpose  : Reset before exiting the program
//            Do your clean up of memory here
//            This is called once just before the game exits
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void shutdown( void )
{
    // Reset to white text on black background
    colour(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

    g_Console.clearBuffer();
}

//--------------------------------------------------------------
// Purpose  : Getting all the key press states
//            This function checks if any key had been pressed since the last time we checked
//            If a key is pressed, the value for that particular key will be true
//
//            Add more keys to the enum in game.h if you need to detect more keys
//            To get other VK key defines, right click on the VK define (e.g. VK_UP) and choose "Go To Definition" 
//            For Alphanumeric keys, the values are their ascii values (uppercase).
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void getInput( void )
{    
    g_abKeyPressed[K_UP]     = isKeyPressed(VK_UP);
    g_abKeyPressed[K_DOWN]   = isKeyPressed(VK_DOWN);
    g_abKeyPressed[K_LEFT]   = isKeyPressed(VK_LEFT);
    g_abKeyPressed[K_RIGHT]  = isKeyPressed(VK_RIGHT);
    g_abKeyPressed[K_SPACE]  = isKeyPressed(VK_SPACE);
    g_abKeyPressed[K_ESCAPE] = isKeyPressed(VK_ESCAPE);
}

//--------------------------------------------------------------
// Purpose  : Update function
//            This is the update function
//            double dt - This is the amount of time in seconds since the previous call was made
//
//            Game logic should be done here.
//            Such as collision checks, determining the position of your game characters, status updates, etc
//            If there are any calls to write to the console here, then you are doing it wrong.
//
//            If your game has multiple states, you should determine the current state, and call the relevant function here.
//
// Input    : dt = deltatime
// Output   : void
//--------------------------------------------------------------
void AIMovementLR(g_Object& ai)
{
	ai.Storage_SC[0].Coord.X += ai.Storage_SC[0].direction;
	if (ai.Storage_SC[0].Coord.X > 70 || ai.Storage_SC[0].Coord.X < 1)
	{
		ai.Storage_SC[0].direction *= -1;
	}
}
void AIMovementUD(g_Object& ai)
{
	ai.Storage_SC[1].Coord.Y += ai.Storage_SC[1].direction;
	if (ai.Storage_SC[1].Coord.Y < 1 || ai.Storage_SC[1].Coord.Y > 30)
	{
		ai.Storage_SC[1].direction *= -1;
	}
}
void AIMovementUDLR(g_Object& ai)
{
	if (goingRight)
	{
		ai.Storage_SC[2].direction = 1;
		ai.Storage_SC[2].Coord.X += ai.Storage_SC[2].direction;
		if (ai.Storage_SC[2].Coord.X == 60)
		{
			goingDown = true;
			goingRight = false;
		}
	}

	if (goingLeft)
	{
		ai.Storage_SC[2].direction = -1;
		ai.Storage_SC[2].Coord.X += ai.Storage_SC[2].direction;
		if (ai.Storage_SC[2].Coord.X == 0)
		{
			goingUp = true;
			goingLeft = false;
		}
	}


	if (goingUp)
	{

		ai.Storage_SC[2].direction= -1;
		ai.Storage_SC[2].Coord.Y += ai.Storage_SC[2].direction;
		if (ai.Storage_SC[2].Coord.Y < 1)
		{
			goingRight = true;
			goingUp = false;
		}
	}

	if (goingDown)
	{
		ai.Storage_SC[2].direction = 1;
		ai.Storage_SC[2].Coord.Y += ai.Storage_SC[2].direction;
		if (ai.Storage_SC[2].Coord.Y > 10)
		{

			goingLeft = true;
			goingDown = false;
		}
	}
}

void CollisionDectector()
{
	Stage1.Storage_SC[3];
	for (unsigned short i = 0; i < 3; i++)
	{
		if (player1.Coord.X == Stage1.Storage_SC[i].Coord.X &&
			player1.Coord.Y == Stage1.Storage_SC[i].Coord.Y)
		{
			player1.hp--;
			player1.Coord.X = 1;
			player1.Coord.Y = 1;
			if (player1.hp < 1)
			{
				//Go Game Lose Scene
			}
		}
	}
}
void colorModifierCollision()
{
	if (player1.Coord.X == color1.Coord.X && player1.Coord.Y == color1.Coord.Y)
	{
		color1.visible = false;

		if (!colorChanged)
		{
			colorChanged = true;
			g_sChar.m_bActive = !g_sChar.m_bActive;
		}
	}
}
//HengSoon Code End
void update(double dt)
{
    // get the delta time
    g_dElapsedTime += dt;
    g_dDeltaTime = dt;

    switch (g_eGameState)
    {
        case S_SPLASHSCREEN : splashScreenWait(); // game logic for the splash screen
            break;
        case S_GAME: gameplay(); // gameplay logic when we are in the game
            break;
    }

//HengSoon Code Start
	colorModifierCollision();

	for (unsigned short i = 0; i < 3; i++)
	{
		switch (Stage1.Storage_SC[i].type)
		{
		case enemy1:
			AIMovementLR(Stage1);
			break;
		case enemy2:
			AIMovementUD(Stage1);
			break;
		case enemy3:
			AIMovementUDLR(Stage1);
			break;
		default:
			break;
		}
	}

//HengSoon Code End

}
//--------------------------------------------------------------
// Purpose  : Render function is to update the console screen
//            At this point, you should know exactly what to draw onto the screen.
//            Just draw it!
//            To get an idea of the values for colours, look at console.h and the URL listed there
// Input    : void
// Output   : void
//--------------------------------------------------------------
void render()
{
    clearScreen();      // clears the current screen and draw from scratch 
    switch (g_eGameState)
    {
        case S_SPLASHSCREEN: renderSplashScreen();
            break;
        case S_GAME: renderGame();
            break;
    }
    renderFramerate();  // renders debug information, frame rate, elapsed time, etc
    renderToScreen();   // dump the contents of the buffer to the screen, one frame worth of game
}

void splashScreenWait()    // waits for time to pass in splash screen
{
    if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
        g_eGameState = S_GAME;
}

void gameplay()            // gameplay logic
{
    processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
    moveCharacter();    // moves the character, collision detection, physics, etc
                        // sound can be played here too.
}

void moveCharacter()
{
    bool bSomethingHappened = false;
    if (g_dBounceTime > g_dElapsedTime)
        return;

    // Updating the location of the character based on the key press
    // providing a beep sound whenver we shift the character
	if (g_abKeyPressed[K_UP] && player1.Coord.Y > 0 && maze[player1.Coord.Y - 1][player1.Coord.X] != 1)
	{
		//Beep(1440, 30);
		player1.Coord.Y--;
		bSomethingHappened = true;

	}
	if (g_abKeyPressed[K_LEFT] && player1.Coord.X > 0 && maze[player1.Coord.Y][player1.Coord.X - 1] != 1)
	{
		//Beep(1440, 30);
		player1.Coord.X--;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_DOWN] && player1.Coord.Y < g_Console.getConsoleSize().Y - 1 && maze[player1.Coord.Y + 1][player1.Coord.X] != 1)
	{
		//Beep(1440, 30);
		player1.Coord.Y++;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_RIGHT] && player1.Coord.X < g_Console.getConsoleSize().X - 1 && maze[player1.Coord.Y][player1.Coord.X + 1] != 1)
	{
		//Beep(1440, 30);
		player1.Coord.X++;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_SPACE])
	{
		g_sChar.m_bActive = !g_sChar.m_bActive;
		bSomethingHappened = true;
	}

	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.125; // 125ms should be enough
	}
}
void processUserInput()
{
    // quits the game if player hits the escape key
    if (g_abKeyPressed[K_ESCAPE])
        g_bQuitGame = true;    
}

void clearScreen()
{
    // Clears the buffer with this colour attribute
    g_Console.clearBuffer(0x1F);
}

void renderSplashScreen()  // renders the splash screen
{
    COORD c = g_Console.getConsoleSize();
    c.Y /= 3;
    c.X = c.X / 2 - 9;
    g_Console.writeToBuffer(c, "A game in 3 seconds", 0x03);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 20;
    g_Console.writeToBuffer(c, "Press <Space> to change character colour", 0x09);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);
}

void renderEnemy()
{
	//HengSoon Code Start

	WORD enemyColor = 0x0C;
	for (unsigned int i = 0; i < 3; i++)
	{
		g_Console.writeToBuffer(Stage1.Storage_SC[i].Coord, (char)2, enemyColor);
	}

	//HengSoon Code End
}

void renderColorModifier(ColorModifier& colorModifier)
{
	if (colorModifier.visible)
	{
		g_Console.writeToBuffer(color1.Coord, 'B', 0x1E); // This is the color modifier's color (Yellow)
	}
}


void renderGame()
{
    renderMap();        // renders the map to the buffer first
    renderCharacter();  // renders the character into the buffer
	renderEnemy();
	renderColorModifier(color1);
}

void renderMap()
{
    // Set up sample colours, and output shadings
    const WORD colors[] = {
        0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
        0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
    };

   /* COORD c;
    for (int i = 0; i < 12; ++i)
    {
        c.X = 5 * i;
        c.Y = i + 1;
        colour(colors[i]);
        g_Console.writeToBuffer(c, " °±²Û", colors[i]);
    }*/

	//Render the level 1 of the game
	COORD c;
	int k = 5;
	for (int i = 0; i < 20; i++)
	{
		c.Y = 0 + i;
		for (int j = 0; j < 60; j++)
		{
			c.X = 0 + j;
			if (maze[i][j] == 1)
			{
				g_Console.writeToBuffer(c, '#', colors[k]);
			}
			else if (maze[i][j] == 2)
			{
				g_Console.writeToBuffer(c, '!', colors[k]);
			}
			else
			{
				g_Console.writeToBuffer(c, ' ', colors[k]);
			}

		}

	}

	COORD end;
	for (int i = 0; i < 1; i++)
	{
		if (g_sChar.m_cLocation.X == 58 && g_sChar.m_cLocation.Y == 1)
		{
			win = true;
			if (win == true)
			{
				end.X = 65;
				end.Y = 10;
				g_Console.writeToBuffer(end, "You WIN", colors[k]);
				system("pause");
			}
		}

	}
}

void renderCharacter()
{
	// Draw the location of the character
	WORD charColor = 0x0C;
	if (g_sChar.m_bActive)
    {
        charColor = 0x0A;
    }
    g_Console.writeToBuffer(player1.Coord, (char)1, charColor);
}

void renderFramerate()
{
    COORD c;
    // displays the framerate
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << 1 / g_dDeltaTime << "fps";
    c.X = g_Console.getConsoleSize().X - 9;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str());

    // displays the elapsed time
    ss.str("");
    ss << g_dElapsedTime << "secs";
    c.X = 0;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str(), 0x59);
}
void renderToScreen()
{
    // Writes the buffer to the console, hence you will see what you have written
    g_Console.flushBufferToConsole();
}
