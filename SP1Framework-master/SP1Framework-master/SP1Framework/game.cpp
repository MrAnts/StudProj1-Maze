// This is the main file for the game logic and function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

using std::string;
using std::ifstream;
using std::ofstream;

ifstream readFile;
ofstream write;

double  g_dElapsedTime;
double  g_dDeltaTime;
bool    g_abKeyPressed[K_COUNT];
bool	inGame = false;
char	output = ' ';

// Game specific variables here
SGameChar   g_sChar;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once

// Console object
Console g_Console(80, 30, "RGB Maze");

const int row = 20;		//<----------The MAP Length and Height
const int col = 60;
int maze[row][col] = {};

//Array for all the coordinates for Stupid Computer AI and ColorBall
short SC_CoordX[4] = {30,45,30,30};
short SC_CoordY[4] = {1,15,10,9};

short CM_CoordX[2] = {1,1};
short CM_CoordY[2] = { 4, 5 };


//Win and Lose Bool
bool win = false;

enum myType
{
	enemy1, //enemy1 is enemy that goes Left and Right
	enemy2, //enemy2 is enemy that goes Up and down
	enemy3, //enemy3 is enemy that goes all 4 direction
	enemy4, //enemy4 is enemy that chase player
};

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
};

short CM_color[2] = { blue, green };

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
		type = enemy1;
		colour(0x1A);	// default colour of the enemy
	}
};

struct colorModifier
{
	colorScheme color;
	COORD Coord;
	bool visible = true;	// check whether color ball/modifier is visible on screen
};

struct endPoint{
	COORD endPointCoord;
	bool visible = true;
	endPoint()
	{
		endPointCoord.X = 1;
		endPointCoord.Y = 2;
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
void StoreCoord(colorModifier& colorObj, short CoordX, short CoordY)
{
	colorObj.Coord.X = CoordX;
	colorObj.Coord.Y = CoordY;
}

struct g_Object      //Struct g_Object is like a container storing all the other game object in the game
{
	//Storage_SC 0 is for Enemy that only walk left right, 
	//Storage_SC 1 is for Enemy that only walk Up down,
	//Storage_SC 2 is for Enemy that can walk all 4 direction.
	//Storage_SC 3 is for Enemy that chase after player.

	StupidComputer Storage_SC[4]; // Numbers of AI in the game
	colorModifier Storage_CM[2]; // Numbers of ColorModifier in the game
	Player player1;
	myType EnemyType = enemy1; // 1st enemy type (enemy1) = enemy that move left and right only
	
	g_Object()
	{	
		//making the 3 enemy in the game into 3 different type, Left right movement enemy, 
		//up down movement enemy, and all 4 direction movement enemy
		for (unsigned int i = 0; i < 4; i++)
		{
			StoreCoord(Storage_SC[i], SC_CoordX[i], SC_CoordY[i]);
			EnemyType = static_cast<myType>(enemy1 + i);
			Storage_SC[i].type = EnemyType;
		}
		for (unsigned int i = 0; i < 2; i++)
		{
			StoreCoord(Storage_CM[i], CM_CoordX[i], CM_CoordY[i]);
		}
	}
};

g_Object Stage1;
Player player1;
endPoint endPointpoint;

// Stage1 means whats is the stage now and that is the place to store all the item that required in the map
// color1 means the colour modifier in the game, that can change player's colour
bool goingRight = false, goingLeft = true, goingUp = false, goingDown = false;
bool collided = false;
int RandDirection = 3, slow = 0, colorchanged = 0;

//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------

void init( void )
{
	mazeread("maze.txt");	//initialise the map from text file

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

// AIMovementLR is the func for enemy1 movement Hard coded path

void AIMovementLR(g_Object& ai)
{
	ai.Storage_SC[0].Coord.X += ai.Storage_SC[0].direction;
	if (maze[(ai.Storage_SC[0].Coord.Y)][(ai.Storage_SC[0].Coord.X - 1)] == 1 || maze[(ai.Storage_SC[0].Coord.Y)][(ai.Storage_SC[0].Coord.X + 1)] == 1)
	{
		ai.Storage_SC[0].direction *= -1;
	}
}

// AIMovementUD is the func for the enemy2 movement Hard coded path

void AIMovementUD(g_Object& ai)
{
	ai.Storage_SC[1].Coord.Y += ai.Storage_SC[1].direction;
	if (maze[(ai.Storage_SC[1].Coord.Y - 1)][(ai.Storage_SC[1].Coord.X)] == 1 || maze[(ai.Storage_SC[1].Coord.Y+1)][(ai.Storage_SC[1].Coord.X)] == 1)
	{
		ai.Storage_SC[1].direction *= -1;
	}
}

//AIMovementUDLR is the func for the enemy3 movement (Random path)
void AIMovementUDLR(g_Object& ai)
{
	switch (RandDirection)
	{
		case 1: //move up
				if (maze[(ai.Storage_SC[2].Coord.Y - 1)][(ai.Storage_SC[2].Coord.X)] != 1)
				{
					ai.Storage_SC[2].direction = -1;
					ai.Storage_SC[2].Coord.Y += ai.Storage_SC[2].direction;
				}
				else
				{
					RandDirection = 1 + rand() % 4;
				}
		break;

		case 2: //move down
				if (maze[(ai.Storage_SC[2].Coord.Y + 1)][(ai.Storage_SC[2].Coord.X)] != 1)
				{
					ai.Storage_SC[2].direction = 1;
					ai.Storage_SC[2].Coord.Y += ai.Storage_SC[2].direction;
				}
				else
				{
					RandDirection = 1 + rand() % 4;
				}
		break;

		case 3: //move left 
				if (maze[(ai.Storage_SC[2].Coord.Y)][(ai.Storage_SC[2].Coord.X - 1)] != 1)
				{
					ai.Storage_SC[2].direction = -1;
					ai.Storage_SC[2].Coord.X += ai.Storage_SC[2].direction;
				}
				else
				{
					RandDirection = 1 + rand() % 4;
				}
		break;

		case 4: //move left 
				if (maze[(ai.Storage_SC[2].Coord.Y)][(ai.Storage_SC[2].Coord.X + 1)] != 1)
				{
					ai.Storage_SC[2].direction = 1;
					ai.Storage_SC[2].Coord.X += ai.Storage_SC[2].direction;
				}
				else
				{
					RandDirection = 1 + rand() % 4;
				}
		break;
	}
	
}

void AIMovementHoming(g_Object& ai)
{

	if (player1.Coord.X < ai.Storage_SC[3].Coord.X && maze[(ai.Storage_SC[3].Coord.Y)][(ai.Storage_SC[3].Coord.X - 1)] != 1)
	{
		ai.Storage_SC[3].direction = -1;
		ai.Storage_SC[3].Coord.X += ai.Storage_SC[3].direction;
	}
	if (player1.Coord.X > ai.Storage_SC[3].Coord.X && maze[(ai.Storage_SC[3].Coord.Y)][(ai.Storage_SC[3].Coord.X + 1)] != 1)// work
	{
		ai.Storage_SC[3].direction = 1;
		ai.Storage_SC[3].Coord.X += ai.Storage_SC[3].direction;
	}
	if (player1.Coord.Y < ai.Storage_SC[3].Coord.Y && maze[(ai.Storage_SC[3].Coord.Y - 1)][(ai.Storage_SC[3].Coord.X)] != 1)
	{
		ai.Storage_SC[3].Ydirection = -1;
		ai.Storage_SC[3].Coord.Y += ai.Storage_SC[3].Ydirection;
	}
	if (player1.Coord.Y > ai.Storage_SC[3].Coord.Y && maze[(ai.Storage_SC[3].Coord.Y + 1)][(ai.Storage_SC[3].Coord.X)] != 1)
	{
		ai.Storage_SC[3].Ydirection = 1;
		ai.Storage_SC[3].Coord.Y += ai.Storage_SC[3].Ydirection;
	}
	
}

void CollisionDectector()
{
	for (unsigned short i = 0; i < 4; i++)
	{
		if (player1.Coord.X == Stage1.Storage_SC[i].Coord.X &&
			player1.Coord.Y == Stage1.Storage_SC[i].Coord.Y)
		{
			player1.hp--;
			player1.Coord.X = 1;	//resetting player X location
			player1.Coord.Y = 1;	//resetting player Y location
			if (player1.hp < 1)
			{
				//Go Game Lose Scene
			}
		}
	}
}

// The function that carries out what happens when the player collidies with the color ball/modifier
void colorModifierCollision()
{
	for (unsigned short i = 0; i < 2; i++)
	{
		if (player1.Coord.X == Stage1.Storage_CM[i].Coord.X && player1.Coord.Y == Stage1.Storage_CM[i].Coord.Y && Stage1.Storage_CM[i].visible == true)
		{
			Stage1.Storage_CM[i].visible = false;
			colorchanged++;
			switch (colorchanged)
			{
				case 1: g_sChar.m_bActive = !g_sChar.m_bActive; // Light Blue to Light Yellow
				break;

				case 2: g_sChar.m_bActive2 = !g_sChar.m_bActive2; // Light Yellow to Light Aqua
				break;
			}
		}
	}
	if (player1.Coord.X == endPointpoint.endPointCoord.X && player1.Coord.Y == endPointpoint.endPointCoord.Y && colorchanged == 2) // Checks if player's coordinates sync with end point as well as if the player's color sync with the end point
	{
		g_bQuitGame = true; // exit game if both conditions are met
	}
}

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
	
	CollisionDectector();
	colorModifierCollision();
	slow++;

	if (slow % 5 == 0)
	{
		// Setting movement for the enemy
		for (unsigned short i = 0; i < 4; i++)
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
			case enemy4:
				AIMovementHoming(Stage1);
				break;
			}
		}
	}

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
		//g_sChar.m_bActive = !g_sChar.m_bActive;//space to change color, removed cause of color modifiers
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

// Rendering enemy func
void renderEnemy()
{
	WORD enemyColor = 0x0C;
	for (unsigned int i = 0; i < 4; i++)
	{
		g_Console.writeToBuffer(Stage1.Storage_SC[i].Coord, (char)2, enemyColor);
	}
}

//Function for render of color ball/modifier
void renderColorModifier()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		if (Stage1.Storage_CM[i].visible)
		{
			g_Console.writeToBuffer(Stage1.Storage_CM[i].Coord, 'B', CM_color[i]); // This is the color modifier's color (Yellow)
		}
	}
}

void renderEndPoint(endPoint& endPoint)
{
	g_Console.writeToBuffer(endPointpoint.endPointCoord, '3', cyan); // This is the end point
}

void renderGame()
{
    renderMap();        // renders the map to the buffer first
    renderCharacter();  // renders the character into the buffer
	renderEnemy();		// renders the enemy into the buffer
	renderColorModifier();// renders color ball/modifier
	renderEndPoint(endPointpoint); //renders the end point
}

void renderMap()
{

	// Set up sample colours, and output shadings
	const WORD colors[] = {
		0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
		0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
	};


	//Render the level 1 of the game
	COORD c;	//initialise coord c for printing of maze

	for (int i = 0; i < row; i++)	//forloop for the rows of the maze
	{
		c.Y = 0 + i;	//check the individual rows
		for (int j = 0; j < col; j++)	//forloop for the columns of the maze
		{
			c.X = 0 + j;	//check individual columns
			if (maze[i][j] == 1)	//if number 1 is found, changed to wall symbol
			{
				g_Console.writeToBuffer(c, 232);
			}
			else if (maze[i][j] == 2)	//if number 2 is found, changed to exit symbol
			{
				g_Console.writeToBuffer(c, '!');
			}
			else		//if anything else, changed to spaces
			{
				g_Console.writeToBuffer(c, ' ');
			}

		}
	}



	//To render win text when win condition happens
	COORD end;
	int k = 5;	//for the text color
	for (int i = 0; i < 1; i++)		//run once for the text to appear
	{
		if (player1.Coord.X == 58 && player1.Coord.Y == 1)	//Temp- check for player has reached ending point
		{
			win = true;	//win set true
			if (win == true)	//condition if win true then this will run
			{
				end.X = 65;	//set coords for the text
				end.Y = 10;	//set coords for the text
				g_Console.writeToBuffer(end, "You WIN", colors[k]);	//render the text through buffer
				system("pause");	//temp- pause for now when game ends
			}
		}

	}

}

void mazeread(std::string load)	//read map from text file function
{
	readFile.open(load);	//open / loads the map which is in Init

	if (readFile.is_open())	//if file = open
	{
		for (int i = 0; i < row; i++)	//check rows
		{
			for (int j = 0; j < col; j++)	//check columns
			{
				readFile >> output;	//converts int in text file to char
				if (output == '1')	//if '1' is found
				{
					maze[i][j] = 1;	//set to 1 which is the walls
				}
				else if (output == '2')	//if '2' is found
				{
					maze[i][j] = 2;	//set to 2 which the end point 
				}
				else if (output == '0')	//if '0' is found
				{
					maze[i][j] = 0;	//set to 0 which is spacings
				}
			}
		}


	}
	readFile.close();	//close file



}

void renderCharacter()
{
	// Draw the location of the character
	//DO NOT GET THIS MIXED UP!
	WORD charColor = blue; // This is the color AFTER colliding with the 1ST color Modifier

	switch (colorchanged)
	{
		case 1: charColor = white; // Light Blue to Light Yellow
		break;

		case 2: charColor = cyan; // Light Yellow to Light Aqua
		break;
	}

	g_Console.writeToBuffer(player1.Coord, 'C', charColor);
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
