#ifndef HEDERI
#define HEDERI

#include "raylib.h"

/**
 * @brief DEBUG is defined for the development version of the game.
 * The game is different when DEBUG is commented out
 */

#define DEBUG

#define ICONS_DEBUG

// Key to exit the program
#define EXITKEY KEY_DELETE

// Print board into console for debug purposes
#define BOARD_DEBUG

// Don't play music if MUTEMUSIC is defined
#define MUTEMUSIC

// Print float (game object) coordinates for debugging
// #define PRINT_FLOAT_POS

// Define texts, windows size, board size (SIZE) - so far the GoL cells are the same width and height
#define WINDOW_TITLE "GoL v2 - version 0.9"

#define TITLE "Title text"

#define OPAS "\n\n[O]Options\n\n[N]otebook"

#define OPTIONS "[ ] Sonar\n\n[ ] Grid\n\n[ ] Chaotic Spawns\n\n[DEL]  Exit game\n\n"

// Cell size should not be changed from 20
#define CELL 20

// this is board size, 40x40
#define SIZE 40 

// Fullscreen is not fully supported as of yet
// #define FULLSCREEN 

// Window dimensions - Scaling is not fully supported yet - 
#define screenWidth 1280
#define screenHeight ((float)screenWidth*0.75) 

// scaling ratio for Textures, etc
#define SCALING ((float)screenWidth/800.f)

// Clock multiplier - affects in game timer
#define CLOCKSPEED 3.0f

// Frames per second
#define HZ 60

// Player movement speed
#define SPEED (CELL*10.f*SCALING)

// Max value to cap the user input generations amount.
#define GEN_MAX 100 

// Merkit tulostukseen / characters to print as stylish choice
#define CELL_CHAR 'i'
#define EMPTY_CELL '_'

// structs are defined here

struct cell{
    int current;   
    int future;
    int species;
    int landtype;
};

struct noteBook{
    int Total[6];
};

struct timers{
    float frames;
    int seconds;
    unsigned int minutes;
};

// Inventory (noteBook) indexes values enum
enum {     
    Coins,
    Baits,
    Perches,
    Pikes,
    Bleaks,
    Trouts
};

// How much Coins fishes are worth
enum {     
    BleaksWorth = 1,
    PerchesWorth = 2,
    PikesWorth = 5,
    TroutsWorth = 50
};

// Enums for the cheatcodes available
enum {     
    NormalMode,
    SonarEnabled,
    GridEnabled,
    SonarGridEnabled,
    ChaoticEnabled,
    ChaoticSonarEnabled,
    ChaoticGridenabled,
    AllCheatsEnabled
};


// function prototypes
void init_board(struct cell board[SIZE][SIZE]);

void fileboard_example(struct cell board[SIZE][SIZE], char *error, unsigned int choose_board, int pop);

void playerMovement(float *ballposx, float *ballposy, struct cell board[SIZE][SIZE]);

void randomize_board(struct cell board[SIZE][SIZE],int pop);

float TitleScaling(float NewWidth);

float lineRatioX(float mouseX,float playerX,float mouseY, float playerY);

float lineRatioY(float mouseX,float playerX,float mouseY, float playerY);

int mousePressOptions(Vector2 mousePos, int Optionsmenu);

int mousePressInventory(Vector2 mousePos, int inventory_on);

void reset_board(struct cell board[SIZE][SIZE]);

int count_ne(int x, int y, int pop,struct cell board[SIZE][SIZE]);

void nextGen(struct cell board[SIZE][SIZE]);

void init_board_species(struct cell board[SIZE][SIZE],int pop);

void landscape(struct cell board[SIZE][SIZE]);

void renderLandscape(struct cell board[SIZE][SIZE], Texture2D ShorePic, Texture2D StatusBar, Texture2D StatusBarCorner);

int CheckFishingArea(Vector2 position,struct cell board[SIZE][SIZE],int erase,int cheats);

void renderIsland(struct cell board[SIZE][SIZE], Texture2D Islandcorner, Texture2D Islandpic,Texture2D rock);

void renderFoliage(struct cell board[SIZE][SIZE], Texture2D xmastree, Texture2D weed);

void render_board(struct cell board[SIZE][SIZE],int Cheatcodes);

void checkLandscape(int x, int y, struct cell board[SIZE][SIZE]);

Color tintColor(int inventory_on);

Color tintColor2_Gray(int inventory_on);

void save_records(struct noteBook inventory);

int countCells (struct cell board[SIZE][SIZE]);

#endif