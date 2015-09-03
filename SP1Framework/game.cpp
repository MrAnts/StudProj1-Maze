// This is the main file for the game logic and function
//
//
#pragma comment(lib, "winmm.lib")
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
char	output = ' ';
char	ascOut = ' ';

// Game specific variables here
SGameChar   g_sChar;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once

// Console object
Console g_Console(80, 30, "RGB Maze");

std::ostringstream ss;
int score = 0;
bool scoreOnce = false;
int highscore[5];

const int row = 20;		//<----------The MAP Length and Height
const int col = 60;

int maze[row][col] = {};

const int arow = 10;
const int acol = 80;

int asciiArt[arow][acol] = {};

int level = 1;

char CM_display[3] = { 'B', 'G', 'R'};

string CurrentColor;
WORD charColor = 0xF9; // This is the default color for player

//Array for all the coordinates for Stupid Computer ai, ColorBall, Description

Description Storage_Description[11];

short SC_CoordX[20] = { 5, 43, 28, 30,   30, 28, 48, 40,    17, 28, 48, 28,   20, 1, 54, 58,    4, 4, 55,43};// each level have 4 enemy, therefore 20 / 4 = 5 levels of enemy
short SC_CoordY[20] = { 12, 5,  9,  9,   12,  9, 10, 12,    18,  9, 10, 13,   17, 7,  7,  2,    3, 15, 3, 5};

short SC_CrazyCoordX[6] = {15,27,44, 18,28,50};
short SC_CrazyCoordY[6] = {4,7,10,    13,14,15};

				      //Level1, Level2, Level3, level 4, level 5, crazy
short CM_CoordX[12] = {18,41,   14,27,  58,47,  54, 19,  16, 47,  64, 20};
short CM_CoordY[12] = {10, 5,   14,1,   18,3,   9,  7,   13, 13,  15,  6};

short CM_color[3] = { blue, green, red };

short Description_CoordX[11] = { 65, 73,    66, 71,      3,  3,  3,      63, 71, 63, 72}; // 2 coordinates comes in a pair, 1 is for the text 1 is for the numbers, example "Hp =" --> 1 coordinate, 3 --> 1 coordinate
short Description_CoordY[11] = {  3,  3,     5,  5,     23, 24, 25,       7,  7,  9,  9};   // 1st pair is level, followed by hp, current color and player.

g_Object Stage;
endPoint endPointpoint;
Player player1;
COORD menuArrow;
COORD menu;
COORD opt;



// Stage means whats is the stage now and that is the place to store all the item that required in the map
// color1 means the colour modifier in the game, that can change player's colour
bool goingRight = false, goingLeft = true, goingUp = false, goingDown = false;
bool collided = false, ButtonPressed = false;
bool levelOneWin = false, levelTwoWin = false, levelThreeWin = false, frenzylevel = false;
int RandDirection = 3, slow = 0, colorchanged = 0, colorCode = 1; // colorcode is for checking which color did the player took 1st

//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
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

void StoreCoord(Description& Descript, short CoordX, short CoordY)
{
	Descript.Coord.X = CoordX;
	Descript.Coord.Y = CoordY;
}

void init( void )
{
	switch (level)
	{
		case 1:mazeread("maze.txt");	//initialise the map from text file
		break;

		case 2:mazeread("maze2.txt");	//initialise the map from text file
		break;

		case 3:mazeread("maze3.txt");	//initialise the map from text file
		break;

		case 4:mazeread("maze4.txt");	//initialise the map from text file
		break;
	}

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

	PlaySound(TEXT("Music.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC); // this is a separate file (Music.wav) in the folder with the game.cpp, without it you cannot run this
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
	g_abKeyPressed[K_ENTER]  = isKeyPressed(VK_RETURN);
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
	if (maze[(ai.Storage_SC[1].Coord.Y - 1)][(ai.Storage_SC[1].Coord.X)] == 1 || maze[(ai.Storage_SC[1].Coord.Y + 1)][(ai.Storage_SC[1].Coord.X)] == 1)
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

//AIMovementHoming is the func for the enemy4 movement (Chasing path)

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

void CrazyAIMovement(g_Object& ai)
{
	for (unsigned short i = 0; i < 6; i++)
	{
		switch (RandDirection)
		{
		case 1: //move up
			if (maze[(ai.Storage_CAI[i].Coord.Y - 1)][(ai.Storage_CAI[i].Coord.X)] != 1)
			{
				ai.Storage_CAI[i].direction = -1;
				ai.Storage_CAI[i].Coord.Y += ai.Storage_CAI[i].direction;
			}
			else
			{
				RandDirection = 1 + rand() % 4;
			}
			break;

		case 2: //move down
			if (maze[(ai.Storage_CAI[i].Coord.Y + 1)][(ai.Storage_CAI[i].Coord.X)] != 1)
			{
				ai.Storage_CAI[i].direction = 1;
				ai.Storage_CAI[i].Coord.Y += ai.Storage_CAI[i].direction;
			}
			else
			{
				RandDirection = 1 + rand() % 4;
			}
			break;

		case 3: //move left 
			if (maze[(ai.Storage_CAI[i].Coord.Y)][(ai.Storage_CAI[i].Coord.X - 1)] != 1)
			{
				ai.Storage_CAI[i].direction = -1;
				ai.Storage_CAI[i].Coord.X += ai.Storage_CAI[i].direction;
			}
			else
			{
				RandDirection = 1 + rand() % 4;
			}
			break;

		case 4: //move right
			if (maze[(ai.Storage_CAI[i].Coord.Y)][(ai.Storage_CAI[i].Coord.X + 1)] != 1)
			{
				ai.Storage_CAI[i].direction = 1;
				ai.Storage_CAI[i].Coord.X += ai.Storage_CAI[i].direction;
			}
			else
			{
				RandDirection = 1 + rand() % 4;
			}
		break;
		}
	}
}

void CollisionDectector()
{
	if (!frenzylevel)
	{
		for (unsigned short i = 0; i < 4; i++)
		{
			if (player1.Coord.X == Stage.Storage_SC[i].Coord.X &&
				player1.Coord.Y == Stage.Storage_SC[i].Coord.Y)
			{
				player1.hp--;
				player1.Coord.X = 1;	//resetting player X location
				player1.Coord.Y = 1;	//resetting player Y location
				if (player1.hp < 1)
				{
					if (!frenzylevel)
					{
						// Reset

						level = 1;
						g_eGameState = S_LOSE;
						frenzylevel = false;
						colorchanged = 0;
						colorCode = 1;
						charColor = 0xF9;
						CurrentColor = "Default";
						for (unsigned int i = 0; i < 12; i++)
						{
							Stage.Storage_CM[i].visible = true;
						}

						StoreCoord(Stage.Storage_SC[3], SC_CoordX[2], SC_CoordY[2]);
						StoreCoord(Stage.Storage_CM[0], CM_CoordX[0], CM_CoordY[0]);
						StoreCoord(Stage.Storage_CM[1], CM_CoordX[1], CM_CoordY[1]);
						renderMap(); // exit game if condition is met
					}
				}
			}
		}
	}
	else
	{
		for (unsigned short i = 0; i < 6; i++)
		{
			if (player1.Coord.X == Stage.Storage_CAI[i].Coord.X &&
				player1.Coord.Y == Stage.Storage_CAI[i].Coord.Y)
			{
				player1.hp--;
				player1.Coord.X = 1;	//resetting player X location
				player1.Coord.Y = 1;	//resetting player Y location
				if (player1.hp < 1)
				{
					g_eGameState = S_LOSE;
					frenzylevel = false;
					renderMap(); // exit game if condition is met
				}
			}
		}
	}
}

// The function that carries out what happens when the player collidies with the color ball/modifier

void colorModifierCollision()
{
	for (unsigned short i = 0; i < 2; i++)
	{
		if (player1.Coord.X == Stage.Storage_CM[i].Coord.X && player1.Coord.Y == Stage.Storage_CM[i].Coord.Y && Stage.Storage_CM[i].visible == true)
		{
			Stage.Storage_CM[i].visible = false;
			colorchanged++;
			if (i == 1)
			{
				colorCode *= -1; // 2nd color collected
			}
			else
			{
				colorCode = 1; // 1st color collected
			}
		}
	}
	if (player1.Coord.X == endPointpoint.endPointCoord.X && player1.Coord.Y == endPointpoint.endPointCoord.Y && colorchanged == 2) // Checks if player's coordinates sync with end point as well as if the player's color sync with the end point
	{
		if (!frenzylevel)
		{
			level++;
			switch (level)
			{
			case 2: mazeread("maze2.txt");
				break;
			case 3: mazeread("maze3.txt");
				break;
			case 4: mazeread("maze4.txt");
				break;
			case 5: mazeread("maze5.txt");
				break;
			case 6: g_eGameState = S_WIN;
				break;
			}

			for (unsigned int i = 0; i < 4; i++)
			{
				switch (level)
				{
				case 2:
					StoreCoord(Stage.Storage_SC[i], SC_CoordX[i + 4], SC_CoordY[i + 4]);
					Stage.EnemyType = static_cast<myType>(enemy1 + i);
					Stage.Storage_SC[i].type = Stage.EnemyType;
					break;

				case 3:
					StoreCoord(Stage.Storage_SC[i], SC_CoordX[i + 8], SC_CoordY[i + 8]);
					Stage.EnemyType = static_cast<myType>(enemy1 + i);
					Stage.Storage_SC[i].type = Stage.EnemyType;
					break;

				case 4:
					StoreCoord(Stage.Storage_SC[i], SC_CoordX[i + 12], SC_CoordY[i + 12]);
					Stage.EnemyType = static_cast<myType>(enemy1 + i);
					Stage.Storage_SC[i].type = Stage.EnemyType;
					break;

				case 5:
					StoreCoord(Stage.Storage_SC[i], SC_CoordX[i + 16], SC_CoordY[i + 16]);
					Stage.EnemyType = static_cast<myType>(enemy1 + i);
					Stage.Storage_SC[i].type = Stage.EnemyType;
					break;
				}
			}
			for (unsigned int i = 0; i < 2; i++)
			{
				switch (level)
				{
				case 2:
					StoreCoord(Stage.Storage_CM[i], CM_CoordX[i + 2], CM_CoordY[i + 2]);
					break;

				case 3:
					StoreCoord(Stage.Storage_CM[i], CM_CoordX[i + 4], CM_CoordY[i + 4]);
					break;

				case 4:
					StoreCoord(Stage.Storage_CM[i], CM_CoordX[i + 6], CM_CoordY[i + 6]);
					break;

				case 5:
					StoreCoord(Stage.Storage_CM[i], CM_CoordX[i + 8], CM_CoordY[i + 8]);
					break;
				}
			}
			for (unsigned int i = 0; i < 9; i++)
			{
				StoreCoord(Storage_Description[i], Description_CoordX[i], Description_CoordY[i]);
			}
			for (unsigned int i = 0; i < 12; i++)
			{
				Stage.Storage_CM[i].visible = true;
			}
			player1.Coord.X = 1;
			player1.Coord.Y = 1;
			colorchanged = 0;
			colorCode = 1;
			charColor = 0xF9;
			CurrentColor = "Default";
		}
		else
		{
			player1.Coord.X = 1;
			player1.Coord.Y = 1;
			colorchanged = 0;
			colorCode = 1;
			charColor = 0xF9;
			CurrentColor = "Default";
			g_eGameState = S_WIN;
		}
	}
}

void backtomenu()
{
	if (g_eGameState == S_WIN)
	{
		if (g_abKeyPressed[K_SPACE])
		{
			g_eGameState = S_SPLASHSCREEN;
		}
	}
	else if (g_eGameState == S_LOSE)
	{
		if (g_abKeyPressed[K_SPACE])
		{
			g_eGameState = S_SPLASHSCREEN;
		}
	}
	else if (g_eGameState == S_INSTRUCTIONS)
	{
		if (g_abKeyPressed[K_SPACE])
		{
			g_eGameState = S_SPLASHSCREEN;
		}
	}
}

void Instructions()	//instructions page
{
	COORD inst;
	inst.X = g_Console.getConsoleSize().X / 2 - 5;
	inst.Y = 5;
	g_Console.writeToBuffer(inst, "Instructions", red);
	inst.X = 1;
	inst.Y += 5;
	g_Console.writeToBuffer(inst, "Movement for player :  Up, Down, Left and Right arrow key", red);
	inst.Y += 1;
	g_Console.writeToBuffer(inst, "Gameplay : Avoid enemies while trying to get to ending point!", red);
	inst.Y += 1;
	g_Console.writeToBuffer(inst, "Objective : Get specific colors by collecting multiple colors around the map", red);
	inst.Y += 5;
	inst.X = g_Console.getConsoleSize().X / 2 - 17;
	g_Console.writeToBuffer(inst, "Press space to return to main menu", red);

}

void optionswait()
{
	if (g_abKeyPressed[K_ENTER])
	{
		if (optchoose == 1)
		{
			waveOutSetVolume(0, 0xFFFFFFFF);
		}
		if (optchoose == 2)
		{
			waveOutSetVolume(0, 0);
		}
	}
	if (g_abKeyPressed[K_SPACE])
	{
		g_eGameState = S_SPLASHSCREEN;
	}
	if (g_abKeyPressed[K_UP] && optchoose > 1)
	{
		optchoose--;
		opt.Y--;
		Sleep(120);
	}
	if (g_abKeyPressed[K_DOWN] && optchoose < 2)
	{
		optchoose++;
		opt.Y++;
		Sleep(120);
	}


}

void asciiRead(std::string ascii)
{
	readFile.open(ascii);	//open / loads the map which is in Init

	if (readFile.is_open())	//if file = open
	{
		for (int i = 0; i < arow; i++)	//check rows
		{
			for (int j = 0; j < acol; j++)	//check columns
			{
				readFile >> ascOut;	//converts int in text file to char
				if (ascOut == '0')	//if '1' is found
				{
					asciiArt[i][j] = 0;	//set to 1 which is the walls
				}
				else if (ascOut == '1')	//if '2' is found
				{
					asciiArt[i][j] = 1;	//set to 2 which the end point 
				}
				else if (ascOut == '2')	//if '0' is found
				{
					asciiArt[i][j] = 2;	//set to 3 which is spacings
				}
				else if (ascOut == '3')
				{
					asciiArt[i][j] = 3;
				}
				else if (ascOut == '4')
				{
					asciiArt[i][j] = 4;
				}
				else if (ascOut == '5')
				{
					asciiArt[i][j] = 5;
				}
				else if (ascOut == '6')
				{
					asciiArt[i][j] = 6;
				}
			}
		}


	}
	readFile.close();	//close file
}

void renderScoreAndHighScore()
{
	if (player1.hp < 1 || level == 6)
	{
		g_Console.clearBuffer(0x1F);

		// score algorithm
		g_Console.writeToBuffer(33, 5, "Your Score: ");

		if (g_dElapsedTime < 300)
		{
			if (!scoreOnce)
			{
				score = (300 - g_dElapsedTime) * 100;
				scoreOnce = true;
			}
		}
		if (g_dElapsedTime > 300 && g_dElapsedTime < 600)
		{
			for (int i = 0; i < 600; ++i)
			{
				if (!scoreOnce)
				{
					score = (600 - g_dElapsedTime) * 10;
					scoreOnce = true;
				}
			}
		}
		if (g_dElapsedTime > 600)
		{
			g_Console.writeToBuffer(15, 5, "No points. You took too long. Better luck next time!");
		}

		// high score algorithm
		if (score > highscore[0])
		{
			highscore[0] = score;
		}
		if (score > highscore[1] && highscore[0] > score)
		{
			highscore[1] = score;
		}
		if (score > highscore[2] && highscore[1] > score)
		{
			highscore[2] = score;
		}
		if (score > highscore[3] && highscore[2] > score)
		{
			highscore[3] = score;
		}
		if (score > highscore[4] && highscore[3] > score)
		{
			highscore[4] = score;
		}

		// prints high score to screen from a .txt file
		ifstream inputFile("highscores.txt");
		if (inputFile.is_open())
		{
			g_Console.writeToBuffer(33, 7, "High Scores: ");
			inputFile >> highscore[0];
			ss.str("");
			ss << highscore[0] << " points";
			g_Console.writeToBuffer(33, 8, "1. ");
			g_Console.writeToBuffer(36, 8, ss.str());
			inputFile >> highscore[1];
			ss.str("");
			ss << highscore[1] << " points";
			g_Console.writeToBuffer(33, 9, "2. ");
			g_Console.writeToBuffer(36, 9, ss.str());
			inputFile >> highscore[2];
			ss.str("");
			ss << highscore[2] << " points";
			g_Console.writeToBuffer(33, 10, "3. ");
			g_Console.writeToBuffer(36, 10, ss.str());
			inputFile >> highscore[3];
			ss.str("");
			ss << highscore[3] << " points";
			g_Console.writeToBuffer(33, 11, "4. ");
			g_Console.writeToBuffer(36, 11, ss.str());
			inputFile >> highscore[4];
			ss.str("");
			ss << highscore[4] << " points";
			g_Console.writeToBuffer(33, 12, "5. ");
			g_Console.writeToBuffer(36, 12, ss.str());
			inputFile.close();
		}

		// save high scores to a .txt file
		ofstream outputFile("highscores.txt");
		if (outputFile.is_open())
		{
			outputFile << "High Scores: " << std::endl;
			outputFile << "1. " << highscore[0] << std::endl;
			outputFile << "2. " << highscore[1] << std::endl;
			outputFile << "3. " << highscore[2] << std::endl;
			outputFile << "4. " << highscore[3] << std::endl;
			outputFile << "5. " << highscore[4] << std::endl;
			outputFile.close();
		}
		ss.str("");
		ss << (score * level) << " points";
		g_Console.writeToBuffer(45, 5, ss.str());
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
		case S_INSTRUCTIONS: backtomenu();	
			break;
        case S_GAME: gameplay(); // gameplay logic when we are in the game
            break;
		case S_WIN: backtomenu();
			break;
		case S_LOSE: backtomenu();
			break;
		case S_OPTIONS: optionswait();
			break;
    }
	
	CollisionDectector();
	colorModifierCollision();

	slow++;
	if (!frenzylevel)
	{
		if (g_eGameState != S_SPLASHSCREEN)
		{
			if (slow % 5 == 0) // speed of the AI
			{
				ButtonPressed = false;
				// Setting movement for the enemy
				for (unsigned short i = 0; i < 4; i++)
				{
					switch (Stage.Storage_SC[i].type)
					{
					case enemy1:
						AIMovementLR(Stage);
						break;
					case enemy2:
						AIMovementUD(Stage);
						break;
					case enemy3:
						AIMovementUDLR(Stage);
						break;
					case enemy4:
						AIMovementHoming(Stage);
						break;
					}
				}
			}
		}
		else
		{
			switch (level)
			{
			  case 1 :
				StoreCoord(Stage.Storage_SC[3], SC_CoordX[2], SC_CoordY[2]);
			  break;

			  case 2 :
				  StoreCoord(Stage.Storage_SC[3], SC_CoordX[6], SC_CoordY[6]);
			  break;

			  case 3:
				  StoreCoord(Stage.Storage_SC[3], SC_CoordX[10], SC_CoordY[10]);
			  break;

			  case 4:
				  StoreCoord(Stage.Storage_SC[3], SC_CoordX[14], SC_CoordY[14]);
			  break;

			  case 5:
				  StoreCoord(Stage.Storage_SC[3], SC_CoordX[18], SC_CoordY[18]);
			  break;
			}
		}
	}
	else
	{
		if (g_eGameState != S_SPLASHSCREEN)
		{
			if (slow % 5 == 0) // speed of the AI
			{
				ButtonPressed = false;
				for (unsigned short i = 0; i < 6; i++)
				{
					switch (Stage.Storage_CAI[i].type)
					{
					case enemy1:
						AIMovementLR(Stage);
						break;
					case enemy2:
						AIMovementUD(Stage);
						break;
					case enemy3:
						CrazyAIMovement(Stage);
						break;
					case enemy4:
						AIMovementHoming(Stage);
						break;
					}
				}
			}
		}
		else
		{
			StoreCoord(Stage.Storage_SC[3], SC_CoordX[15], SC_CoordY[15]);
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


void splashScreenWait()    // waits for time to pass in splash screen
{
	//if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
	//    g_eGameState = S_GAME;
	if (g_abKeyPressed[K_ENTER])
	{
		if (choice == 1)
		{
			g_eGameState = S_GAME;
			player1.hp = 3;
			player1.Coord.X = 1;
			player1.Coord.Y = 1;
			scoreOnce = false;
			mazeread("maze.txt");
		}
		if (choice == 2)
		{
			player1.hp = 1;
			frenzylevel = true;
			g_eGameState = S_GAME;
			player1.Coord.X = 1;
			player1.Coord.Y = 1;
		}
		if (choice == 3)
		{
			g_eGameState = S_INSTRUCTIONS;
		}
		if (choice == 4)
		{
			g_eGameState = S_OPTIONS;
		}
		if (choice == 5)
		{
			g_bQuitGame = true;
		}
	}
	if (g_abKeyPressed[K_UP] && choice > 1)
	{
		choice--;
		menu.Y--;
		Sleep(120);

	}
	if (g_abKeyPressed[K_DOWN] && choice < 5)
	{
		choice++;
		menu.Y++;
		Sleep(120);

	}
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
		Beep(1440, 30);
		player1.Coord.Y--;
		bSomethingHappened = true;

	}
	if (g_abKeyPressed[K_LEFT] && player1.Coord.X > 0 && maze[player1.Coord.Y][player1.Coord.X - 1] != 1)
	{
		Beep(1440, 30);
		player1.Coord.X--;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_DOWN] && player1.Coord.Y < g_Console.getConsoleSize().Y - 1 && maze[player1.Coord.Y + 1][player1.Coord.X] != 1)
	{
		Beep(1440, 30);
		player1.Coord.Y++;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_RIGHT] && player1.Coord.X < g_Console.getConsoleSize().X - 1 && maze[player1.Coord.Y][player1.Coord.X + 1] != 1)
	{
		Beep(1440, 30);
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
    g_Console.clearBuffer(0x1E);
}

void renderSplashScreen()  // renders the splash screen
{
	COORD menu = g_Console.getConsoleSize();
	menu.X /= 2;
	menu.Y = menu.Y / 2 - 5;

	g_Console.writeToBuffer(menu, "RGB Maze", red);
}

// Rendering enemy func
void renderEnemy()
{
	WORD enemyColor = 0x0C;
	if (!frenzylevel)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			g_Console.writeToBuffer(Stage.Storage_SC[i].Coord, (char)2, enemyColor);
		}
	}
	else
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			g_Console.writeToBuffer(Stage.Storage_CAI[i].Coord, (char)2, enemyColor);
		}
	}
}

//Function for render of color ball/modifier
void renderColorModifier()
{
	for (unsigned int i = 0; i < 2; i++)
	{
		if (Stage.Storage_CM[i].visible)
		{
			if (frenzylevel)
			{
				g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i + 1], CM_color[i + 1]); // This is the color modifier's color (Yellow)
			}
			switch (level)
			{ 
				case 1 :	
					g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i], CM_color[i]); // This is the color modifier's color (Yellow)
				break;

				case 2:
					g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i + 1], CM_color[i + 1]); // This is the color modifier's color (Yellow)
				break;

				case 3:
					if (i + 2 > 2)
					{
						g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[0], CM_color[0]);
					}
					else
					{
						g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i + 2], CM_color[i + 2]); // This is the color modifier's color (Yellow)
					}
				break;

				case 4:
					g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i], CM_color[i]); // This is the color modifier's color (Yellow)
				break;

				case 5:
					g_Console.writeToBuffer(Stage.Storage_CM[i].Coord, CM_display[i + 1], CM_color[i + 1]); // This is the color modifier's color (Yellow)
				break;


			}
		}
	}
}

void renderEndPoint(endPoint& endPoint)
{
	switch (level)
	{
		case 1:  
			g_Console.writeToBuffer(endPointpoint.endPointCoord, 'C', cyan); // This is the 1st end point
		break;

		case 2:
			g_Console.writeToBuffer(endPointpoint.endPointCoord, 'M', magenta); // This is the 2nd end point
		break;

		case 3:
			g_Console.writeToBuffer(endPointpoint.endPointCoord, 'Y', yellow); // This is the 3rd end point
		break;

		case 4:
			g_Console.writeToBuffer(endPointpoint.endPointCoord, 'C', cyan); // This is the 2nd end point
		break;

		case 5:
			g_Console.writeToBuffer(endPointpoint.endPointCoord, 'M', magenta); // This is the 3rd end point
		break;
	}

}

void renderMap()
{
	// Set up sample colours, and output shadings
	const WORD colors[] = {
		0x2A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
		0xA3, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
	};

	if (g_eGameState == S_SPLASHSCREEN)
	{

		int color = 0;
		for (int k = 1; k < g_dElapsedTime; k++)
		{
			color++;
			if (color == 12)
			{
				color = 0;
			}
		}
		COORD asc;	//initialise coord c for printing of maze
		asciiRead("title.txt");
		for (int i = 0; i < arow; i++)	//forloop for the rows of the maze
		{
			asc.Y = 0 + i;	//check the individual rows
			for (int j = 0; j < acol; j++)	//forloop for the columns of the maze
			{
				asc.X = 0 + j;	//check individual columns
				if (asciiArt[i][j] == 1)
				{
					g_Console.writeToBuffer(asc, 219, colors[color]);
				}
				else if (asciiArt[i][j] == 0)	//if number 1 is found, changed to wall symbol
				{
					g_Console.writeToBuffer(asc, ' ', 0x1D);
				}
				else if (asciiArt[i][j] == 2)	//if number 2 is found, changed to exit symbol
				{
					g_Console.writeToBuffer(asc, '|', 0x1D);
				}
				else if (asciiArt[i][j] == 3)
				{
					g_Console.writeToBuffer(asc, "\\", 0x1D);
				}
				else if (asciiArt[i][j] == 4)
				{
					g_Console.writeToBuffer(asc, '/', 0x1D);
				}
				else if (asciiArt[i][j] == 5)
				{
					g_Console.writeToBuffer(asc, '<', 0x1D);
				}
				else if (asciiArt[i][j] == 6)
				{
					g_Console.writeToBuffer(asc, '_', 0x1D);
				}
			}
		}
	}
	else if (g_eGameState == S_GAME)
	{
		if (frenzylevel == true)
		{
			mazeread("crazymap.txt");
		}
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
	}
	else if (g_eGameState == S_LOSE)
	{
		renderScoreAndHighScore();
		/*COORD asc;	//initialise coord c for printing of maze
		asciiRead("lose.txt");
		for (int i = 0; i < arow; i++)	//forloop for the rows of the maze
		{
			asc.Y = 0 + i;	//check the individual rows
			for (int j = 0; j < acol; j++)	//forloop for the columns of the maze
			{
				asc.X = 0 + j;	//check individual columns
				if (asciiArt[i][j] == 1)
				{
					g_Console.writeToBuffer(asc, 219, 0x1D);
				}
				else if (asciiArt[i][j] == 0)	//if number 1 is found, changed to wall symbol
				{
					g_Console.writeToBuffer(asc, ' ', 0x1D);
				}
				else if (asciiArt[i][j] == 2)	//if number 2 is found, changed to exit symbol
				{
					g_Console.writeToBuffer(asc, '|', 0x1D);
				}
				else if (asciiArt[i][j] == 3)
				{
					g_Console.writeToBuffer(asc, "\\", 0x1D);
				}
				else if (asciiArt[i][j] == 4)
				{
					g_Console.writeToBuffer(asc, '/', 0x1D);
				}
				else if (asciiArt[i][j] == 5)
				{
					g_Console.writeToBuffer(asc, '_', 0x1D);
				}
			}
		}*/


	}
	else if (g_eGameState == S_WIN)
	{
		COORD asc;	//initialise coord c for printing of maze
		asciiRead("win.txt");
		for (int i = 0; i < arow; i++)	//forloop for the rows of the maze
		{
			asc.Y = 0 + i;	//check the individual rows
			for (int j = 0; j < acol; j++)	//forloop for the columns of the maze
			{
				asc.X = 0 + j;	//check individual columns
				if (asciiArt[i][j] == 1)
				{
					g_Console.writeToBuffer(asc, 219, 0x1D);
				}
				else if (asciiArt[i][j] == 0)	//if number 1 is found, changed to wall symbol
				{
					g_Console.writeToBuffer(asc, ' ', 0x1D);
				}
				else if (asciiArt[i][j] == 2)	//if number 2 is found, changed to exit symbol
				{
					g_Console.writeToBuffer(asc, '|', 0x1D);
				}
				else if (asciiArt[i][j] == 3)
				{
					g_Console.writeToBuffer(asc, "\\", 0x1D);
				}
				else if (asciiArt[i][j] == 4)
				{
					g_Console.writeToBuffer(asc, '/', 0x1D);
				}
				else if (asciiArt[i][j] == 5)
				{
					g_Console.writeToBuffer(asc, '_', 0x1D);
				}
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
	//DO NOT GET THIS MIXED UP

	CurrentColor = "Default";

	switch (level)
	{
		case 1:
			switch (colorchanged)
			{
				case 1: 
					if (colorCode == 1) // 1st color
					{
						charColor = 0xF9; // blue
						CurrentColor = "Blue";
					}
					else // 2nd color
					{
						charColor = 0xFA; // green
						CurrentColor = "Green";
					}
				break;

				case 2: 
						charColor = 0xFB; // Cyan
						CurrentColor = "Cyan";
				break;
			}
		break;

		case 2:
			switch (colorchanged)
			{
			case 1: 				
				if (colorCode == 1) // 1st color
				{
					charColor = 0xFA; // green
					CurrentColor = "Green";
				}
				else // 2nd color
				{
					charColor = 0xFC; //red
					CurrentColor = "Red";
				}
			break;

			case 2:
				charColor = 0xFD; // magenta
				CurrentColor = "Magenta";
			break;
			}
		break;

		case 3:
			switch (colorchanged)
			{
				case 1: 				
					if (colorCode == 1) // 1st color
					{
						charColor = 0xFC; // Red
						CurrentColor = "Red";
					}
					else // 2nd color
					{
						charColor = 0xF9; // Blue
						CurrentColor = "Blue";
					}
				break;


				case 2:
					charColor = 0xFE; // Yellow
					CurrentColor = "Yellow";
				break;
			}
		break;

		case 4:
			switch (colorchanged)
			{
			case 1:
				if (colorCode == 1) // 1st color
				{
					charColor = 0xF9; // blue
					CurrentColor = "Blue";
				}
				else // 2nd color
				{
					charColor = 0xFA; // green
					CurrentColor = "Green";
				} 
				break;

			case 2:
				charColor = 0xFB; // Cyan
				CurrentColor = "Cyan";
				break;
			}
			break;

		case 5:
			switch (colorchanged)
			{
			case 1:
				if (colorCode == 1) // 1st color
				{
					charColor = 0xFA; // green
					CurrentColor = "Green";
				}
				else // 2nd color
				{
					charColor = 0xFC; //red
					CurrentColor = "Red";
				}
				break;

			case 2:
				charColor = 0xFD; // magenta
				CurrentColor = "Magenta";
				break;
			}
			break;
	}

	g_Console.writeToBuffer(player1.Coord, 'P', charColor);
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

void renderInGameText()
{
	if (!frenzylevel)
	{
		g_Console.writeToBuffer(Storage_Description[0].Coord, "Level : ", Storage_Description[0].color);
		g_Console.writeToBuffer(Storage_Description[1].Coord, level + 48, Storage_Description[1].color); // +48 because its a char, 48 refers to 0 in char
		g_Console.writeToBuffer(Storage_Description[2].Coord, "HP : ", Storage_Description[2].color);
		g_Console.writeToBuffer(Storage_Description[3].Coord, player1.hp + 48, Storage_Description[3].color); // +48 because its a char, 48 refers to 0 in char
		g_Console.writeToBuffer(Storage_Description[4].Coord, "R = Red,                 B = Blue,                   G = Green           ", Storage_Description[4].color);
		g_Console.writeToBuffer(Storage_Description[5].Coord, "C = Cyan,                M = Magenta,                Y = Yellow          ", Storage_Description[5].color);
		g_Console.writeToBuffer(Storage_Description[6].Coord, "Cyan = Blue + Greem,     Magenta = Red + Blue,       Yellow = Green + Red", Storage_Description[6].color);
		g_Console.writeToBuffer(Storage_Description[7].Coord, "Color : ", Storage_Description[7].color);
		g_Console.writeToBuffer(Storage_Description[8].Coord, CurrentColor, Storage_Description[8].color);
		g_Console.writeToBuffer(Storage_Description[9].Coord, "Player : ", Storage_Description[9].color);
		g_Console.writeToBuffer(Storage_Description[10].Coord, 'P', charColor);
	}
	else
	{
		g_Console.writeToBuffer(Storage_Description[0].Coord, "HP : ", Storage_Description[2].color);
		g_Console.writeToBuffer(Storage_Description[1].Coord, player1.hp + 48, Storage_Description[3].color); // +48 because its a char, 48 refers to 0 in char
		g_Console.writeToBuffer(Storage_Description[4].Coord, "R = Red,                 B = Blue,                   G = Green           ", Storage_Description[4].color);
		g_Console.writeToBuffer(Storage_Description[5].Coord, "C = Cyan,                M = Magenta,                Y = Yellow          ", Storage_Description[5].color);
		g_Console.writeToBuffer(Storage_Description[6].Coord, "Cyan = Blue + Greem,     Magenta = Red + Blue,       Yellow = Green + Red", Storage_Description[6].color);
		g_Console.writeToBuffer(Storage_Description[7].Coord, "Color : ", Storage_Description[7].color);
		g_Console.writeToBuffer(Storage_Description[8].Coord, CurrentColor, Storage_Description[8].color);
		g_Console.writeToBuffer(Storage_Description[9].Coord, "Player : ", Storage_Description[9].color);
		g_Console.writeToBuffer(Storage_Description[10].Coord, 'P', charColor);
	}

}

void renderToScreen()
{
    // Writes the buffer to the console, hence you will see what you have written
    g_Console.flushBufferToConsole();
}

void menuRender()
{
	renderMap();

	menu.X = g_Console.getConsoleSize().X / 2 - 5;
	menu.Y = g_Console.getConsoleSize().Y / 2;

	g_Console.writeToBuffer(menu, "Play Game", white);
	menu.Y += 1;
	g_Console.writeToBuffer(menu, "Play Frenzy", white);
	menu.Y += 1;
	g_Console.writeToBuffer(menu, "Instructions", white);
	menu.Y += 1;
	g_Console.writeToBuffer(menu, "Options", white);
	menu.Y += 1;
	g_Console.writeToBuffer(menu, "Exit", white);
	switch (choice)
	{
	case 1:
		menu.X = g_Console.getConsoleSize().X / 2 - 5;
		menu.Y = g_Console.getConsoleSize().Y / 2;
		g_Console.writeToBuffer(menu, "Play Game", red);
		break;
	case 2:
		menu.X = g_Console.getConsoleSize().X / 2 - 5;
		menu.Y = g_Console.getConsoleSize().Y / 2 + 1;
		g_Console.writeToBuffer(menu, "Play Frenzy", red);
		break;
	case 3:
		menu.X = g_Console.getConsoleSize().X / 2 - 5;
		menu.Y = g_Console.getConsoleSize().Y / 2 + 2;
		g_Console.writeToBuffer(menu, "Instructions", red);
		break;
	case 4:
		menu.X = g_Console.getConsoleSize().X / 2 - 5;
		menu.Y = g_Console.getConsoleSize().Y / 2 + 3;
		g_Console.writeToBuffer(menu, "Options", red);
		break;
	case 5:
		menu.X = g_Console.getConsoleSize().X / 2 - 5;
		menu.Y = g_Console.getConsoleSize().Y / 2 + 4;
		g_Console.writeToBuffer(menu, "Exit", red);
		break;
	}

}

void menuinit()	//initialised menu arrows
{
	menuArrow.X = 23;
	menuArrow.Y = 11;
}

void options()
{
	opt.X = g_Console.getConsoleSize().X / 2 - 5;
	opt.Y = 5;
	g_Console.writeToBuffer(opt, "Options", red);
	opt.X = 1;
	opt.Y += 5;
	g_Console.writeToBuffer(opt, "Sounds", red);
	opt.Y += 1;
	g_Console.writeToBuffer(opt, "---------", red);
	opt.Y += 1;
	g_Console.writeToBuffer(opt, "Yes", red);
	opt.Y += 1;
	g_Console.writeToBuffer(opt, "No", red);
	opt.Y += 1;
	g_Console.writeToBuffer(opt, "---------", red);
	opt.Y += 5;
	opt.X = g_Console.getConsoleSize().X / 2 - 17;
	g_Console.writeToBuffer(opt, "Press space to return to main menu", red);
	switch (optchoose)
	{
	case 1:
		menu.X = 1;
		menu.Y = 12;
		g_Console.writeToBuffer(menu, "Yes", white);
		break;
	case 2:
		menu.X = 1;
		menu.Y = 13;
		g_Console.writeToBuffer(menu, "No", white);
		break;
	}
}

void win()
{
	COORD won = g_Console.getConsoleSize();
	won.X /= 2;
	won.Y /= 2;

	g_Console.writeToBuffer(won, "YOU WIN", red);
}

void winUpdate()
{
	renderMap();

	COORD win;
	win = g_Console.getConsoleSize();
	win.X /= 2;
	win.Y /= 2;

	g_Console.writeToBuffer(win, "Press space to return to main menu", red);
}

void loseUpdate()
{
	renderMap();
	COORD lose;
	lose = g_Console.getConsoleSize();
	lose.X /= 2;
	lose.Y /= 2;

	g_Console.writeToBuffer(lose, "Press space to return to main menu", red);
}

void render()
{
	clearScreen();      // clears the current screen and draw from scratch 
	switch (g_eGameState)
	{
		case S_SPLASHSCREEN: menuRender();
			break;
		case S_GAME: renderGame();
			break;
		case S_WIN: winUpdate();
			break;
		case S_LOSE: loseUpdate();
			break;
		case S_INSTRUCTIONS: Instructions();
			break;
		case S_OPTIONS: options();
			break;
	}
	renderFramerate();  // renders debug information, frame rate, elapsed time, etc
	renderToScreen();   // dump the contents of the buffer to the screen, one frame worth of game
}

void renderGame()
{
	renderMap();        // renders the map to the buffer first
	renderCharacter();  // renders the character into the buffer
	renderEnemy();		// renders the enemy into the buffer
	renderColorModifier();// renders color ball/modifier
	renderInGameText();
	renderEndPoint(endPointpoint); //renders the end point
	renderScoreAndHighScore();
}

g_Object::g_Object()
{
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
	for (unsigned int i = 0; i < 11; i++)
	{
		StoreCoord(Storage_Description[i], Description_CoordX[i], Description_CoordY[i]);
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		StoreCoord(Storage_CAI[i], SC_CrazyCoordX[i], SC_CrazyCoordY[i]);
		EnemyType = static_cast<myType>(enemy3);
		Storage_CAI[i].type = EnemyType;
	}
}