/*
    GoL V2 by e2403299

    -This code is based on Raylib example below:
        https://github.com/raysan5/raylib/blob/master/examples/core/core_input_keys.c
*/

/** 
 * @file main.c
 * @author e2403299
 * @brief Main program file for gol V2 
 * @date 2025-11-30
 * @details Jontka is a fishing game based on John Conway's Game of Life cell evolution.
 * 
 * -This code is based on Raylib example below:
 *       https://github.com/raysan5/raylib/blob/master/examples/core/core_input_keys.c
 * 
 *  Licensed under the GNU General Public License v3.0 (GPLv3).
*/

#include "header.h"

#include <stdio.h>
#include <raylib.h>
#include <math.h>

// setup random numbers and seed
#include <stdlib.h>
#include <time.h>

/** 
 * @brief The main program for the game
 * @return Exit the main program
*/
int main(void)
{
    // Init window, hide mouse cursor
    #ifdef FULLSCREEN

    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    SetConfigFlags(FLAG_FULLSCREEN_MODE); // May not work optimally

    InitWindow(screenWidth, screenHeight, WINDOW_TITLE);
    #else 
    InitWindow(screenWidth, screenHeight, WINDOW_TITLE);
    #endif

    SetExitKey(KEY_DELETE);  

    // Hide cursor and use hook.png instead if DEBUG not defined
    #ifndef DEBUG
    HideCursor();
    #endif

    // Seed is randomized by time
    srand(time(NULL)); 

    /*
        Define variables for the game start
    */ 

    int CheatCodes = NormalMode;

    // For the float (koho) fishing object offset
    float offset_y = CELL * SCALING / sqrt(3);
    
    #ifdef DEBUG
    CheatCodes = SonarGridEnabled; // Debug mode enables visuals by defeult
    #else
    
    int TitleAlpha = 255; // Title fade animation value
    #endif

    bool insideCircle = false;

    bool Optionsmenu = false;

    bool Notebookmenu = false;

    char error = 0;  // file not found errors

    // init boards and other structs

    struct cell board[SIZE][SIZE] = {0};

    struct timers UpdateCounter = {0};

    struct timers global = {0};

    // Random time if not debug
    #ifndef DEBUG
    global.seconds = rand() % 60;
    global.minutes = rand() % 60;
    #endif
 
    // Inventory init values
    struct noteBook inventory = { .Total = {0,20,0,0,0,0} };

    // List of texts with different colors
    const char *inventoryTexts[] = {"Coins","Baits","Perch","Pikes","Bleaks","???"};

    Color BLEAK_COLOR = {70, 130, 180, 255}; // Color for bleaks
    const Color inventoryColors[] = { GOLD, PINK, DARKGREEN, GREEN, BLEAK_COLOR };

    // random board number
    int randomBoard = rand() % 6 + 1; // only 6 boards so far

    // Load fonts
    Font notebookFont = LoadFontEx("fonts/type.ttf",  24*SCALING, 0, 0);
    if (notebookFont.texture.id == 0) printf("Font not loaded!\n");

    Font infoFont = LoadFontEx("fonts/type.ttf",  20*SCALING, 0, 0);
    if (infoFont.texture.id == 0) printf("Font not loaded!\n");

    /*
        Load textures, picture files  
    */

    //player hat
    Texture2D HatTexture = LoadTexture("img/hat.png"); // Player hat

    // Banners and menu backgrounds
    #ifndef ICONS_DEBUG
    Texture2D TitleTexture = LoadTexture("img/title.png"); // title banner
    #endif

    Texture2D InventoryPic = LoadTexture("img/invent.png"); // inventory

    Texture2D InfoPic= LoadTexture("img/info.png"); // info box top right

    Texture2D NotebookPic = LoadTexture("img/notebook.png"); // notebook menu bg

    Texture2D OptionsMenuPic = LoadTexture("img/options.png"); // notebook menu bg

    // Statusbar tiles
    Texture2D StatusbarPic= LoadTexture("img/MenuTile1.png"); // Top status bar

    Texture2D StatusbarCorner= LoadTexture("img/MenuTile2.png"); // Top status bar corner tile

    // Textures for landscape items
    Texture2D WeedPic= LoadTexture("img/weed.png"); // Island

    Texture2D IslandPic= LoadTexture("img/island.png"); // Island

    Texture2D xmastree= LoadTexture("img/xmastree.png"); // Island

    Texture2D rock = LoadTexture("img/rock.png");

    Texture2D ShorePic= LoadTexture("img/shore.png"); // shore for the edges

    Texture2D IslandCornerPic= LoadTexture("img/islandcorner.png"); // Island corner

    // Icon textures
    Texture2D CoinsPic = LoadTexture("img/Coins.png");

    Texture2D BaitsPic = LoadTexture("img/Baits.png");

    Texture2D PerchesPic = LoadTexture("img/Perches.png");

    #ifndef ICONS_DEBUG
    Texture2D PikesPic = LoadTexture("img/Pikes.png");
    // Mouse cursor icon
    Texture2D HookPic = LoadTexture("img/hook.png"); // Cursor
    #endif

    Texture2D AnchorPic = LoadTexture("img/Anchor.png");

    // Use mousePos vector for mouse position
    Vector2 mousePos = GetMousePosition();

    Vector2 CheckFishingSpot = {0};

    // this array is for inventory text printing
    char inventoryText[100] = "";

    // Target fps is 60 by default, but it can be changed for modern displays > 144Hz
    SetTargetFPS(HZ); 

    // Color for options menu font
    Color beige = {223,221,200,255};

    // Audio
    InitAudioDevice();     // Initialize audio system
    // keep audio in this order

    Sound blop = LoadSound("sfx/blop.wav");

    Sound zip = LoadSound("sfx/zip.wav");

    Sound robo = LoadSound("sfx/robo.wav");

    Sound page = LoadSound("sfx/page.wav");

    Sound cheatSound = LoadSound("sfx/cheats.mp3");

    // Don't play music if MUTEMUSIC is defined
    #ifndef MUTEMUSIC
        Music introMusic = LoadMusicStream("sfx/juu.mp3");
        PlayMusicStream(introMusic);
    #endif

    // inventory is not visible by default
    int inventory_on = false;
    
    // Board initialization 
    #ifdef DEBUG
    fileboard_example(board, &error, 1,Perches);
    fileboard_example(board, &error, 2,Pikes);
    fileboard_example(board, &error, 3,Bleaks);
    #else
    fileboard_example(board, &error, 1,Perches);
    fileboard_example(board, &error, 2,Pikes);
    fileboard_example(board, &error, 3,Bleaks);
    #endif
    
    // init landscape
    landscape(board);

    // Render initial board
    render_board(board,CheatCodes);

    // Init player position
    Vector2 ballPosition = { (float)3*screenWidth/4, (float)3*screenHeight/4 }; // middle of bottom right

    /*  ---------------
        Main game loop.
        ---------------
    */  
    while (!WindowShouldClose())
    {
        // render landscape
        renderLandscape(board,ShorePic,StatusbarPic, StatusbarCorner);
        // void renderLandscape(struct cell board[SIZE][SIZE], Texture2D ShorePic, Texture2D StatusBar, Texture2D StatusBarCorner, Texture2D Islandcorner) {

        // count frames and time, seconds
        UpdateCounter.frames += GetFrameTime();

        if (UpdateCounter.frames >= 1.0f / CLOCKSPEED) // Count seconds
        {
            // Avoid bloating the screen with cells, unless Chaotic mode is active
            if (countCells(board) >= 250){
                if (!(CheatCodes & ChaoticEnabled)){
                    reset_board(board);
                    printf("Reset board!\n");
                }

            }
            nextGen(board); // Board updates function for different species
          
            #ifdef BOARD_DEBUG
            if (IsKeyDown(KEY_L)){
            // print board into terminal
            for(int j = 0; j < SIZE; j++){
                for(int i = 0; i < SIZE; i++){
                    if (board[j][i].current >= 1) printf("%d ",board[j][i].current);
                    else printf("%c ",EMPTY_CELL);
                    }
                printf("\n");
            }
            printf("\n");
            #endif
            }


            UpdateCounter.seconds++;
            UpdateCounter.frames -= 1.0f / CLOCKSPEED; // keep leftover fraction

            // Update visual clock for player
            global.seconds++;

            // Warning for screen dimensions that are not divisable by 20 which affects scaling
            #ifdef DEBUG
            if (screenWidth % 20) printf("\n - Warning! - Screen width is %d and it is not divisable by 20\n",screenWidth);
            #endif
        }

        // Update board interval
        if (UpdateCounter.seconds == 30) {


            // Load different species of gish
            randomBoard = rand() % 6 + 1; // only 6 boards so far
            fileboard_example(board, &error, randomBoard,Perches);

            // Simulate rolling 5 dice and get all with same numbers
            if (rand() % 2){
                randomBoard = rand() % 6 + 1; // Rare chance of spawning Trouts
                fileboard_example(board, &error, randomBoard,Trouts);
            } 

            // Common outcome is spawning Pikes instead
            else {
                randomBoard = rand() % 6 + 1; // only 6 boards so far
                fileboard_example(board, &error, randomBoard,Pikes);
            }

            randomBoard = rand() % 6 + 1; // only 6 boards so far
            fileboard_example(board, &error, randomBoard,Bleaks);


            // Spawn Chaotic random board if ChaoticEnabled
            if (CheatCodes >= ChaoticEnabled) {
                randomize_board(board,Perches);
                randomize_board(board,Pikes);
                randomize_board(board,Bleaks);
            }


            UpdateCounter.seconds = 0; 
        }

        #ifdef MUTEMUSIC
            // Don't play music
        #else
            // Update music
            UpdateMusicStream(introMusic);
        #endif

        // update player movement (Position x and y) in the function
        playerMovement(&ballPosition.x,&ballPosition.y,board);

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        // Background color = tausta
        ClearBackground(SKYBLUE);

        // Render cells and other visuals if cheats are on
        render_board(board,CheatCodes);

        // render landscape elements
        renderIsland(board,IslandCornerPic,IslandPic,rock);

        // Float position into a variable
        CheckFishingSpot.x = (ballPosition.x + lineRatioX(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y));

        CheckFishingSpot.y = (ballPosition.y + offset_y + lineRatioY(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y));

        if (!inventory.Total[Baits] && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))){
             
            if((!inventory_on) && mousePressInventory(mousePos,1)) {
                if ((!Optionsmenu) && mousePressOptions(mousePos, 1)) PlaySound(robo);
                printf("Out of baits! \n\n");
            }
            
        } 
        
        // Check if player pressed on a rectangle which contains fish
        if (CheckFishingArea(CheckFishingSpot,board,false,CheatCodes) && (!inventory_on && !Optionsmenu) && !mousePressInventory(mousePos, inventory_on)
        && !mousePressOptions(mousePos,Optionsmenu) && (!Notebookmenu)){
            // check if player has baits left and inventory is not open
            if (inventory.Total[Baits]  && (!insideCircle)){
                int FishName = CheckFishingArea(CheckFishingSpot,board,true,CheatCodes)+1; // plus 1 to corresponding fish name in the list

                printf("Got a fish! ----- ( %s ) ",inventoryTexts[FishName]); 

                switch (FishName)
                {
                case Bleaks:
                    inventory.Total[Coins] += BleaksWorth;
                    printf(" They are worth: %d\n\n",BleaksWorth);
                    break;
                case Perches:
                    inventory.Total[Coins] += PerchesWorth;
                    printf(" They are worth: %d\n\n",PerchesWorth);
                    break;
                case Pikes:
                    inventory.Total[Coins] += PikesWorth;
                    printf(" They are worth: %d\n\n",PikesWorth);
                    break;
                case Trouts:
                    inventory.Total[Coins] += TroutsWorth;
                    printf(" They are worth: %d\n\n",TroutsWorth);
                    break;
                default:
                    break;
                }
                inventory.Total[FishName]++;

                // Decrease baits
                inventory.Total[Baits]--;

                PlaySound(zip);
            }

        }

        // check if mouse pressed inside a circle of fishing rod
        if  (!lineRatioX(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y) || (!lineRatioY(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y))) {
            insideCircle = true;

            #ifdef DEBUG
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) printf("(Input)\nPressed mouse inside circle!\n");
            #endif
        }

        else insideCircle = false;

        #ifdef DEBUG // print board landscape values into terminal when pressing P
        if (IsKeyPressed(KEY_P)) {
            // print board into terminal
            for(int j = 0; j < SIZE; j++){
                for(int i = 0; i < SIZE; i++){
                    if (board[j][i].landtype >= 1) printf("%d ",board[j][i].landtype);
                    else printf("%c ",EMPTY_CELL);
                    }
                printf("\n");
            }
            printf("\n");
        }
        #endif

        // Float (koho) - render if mouse is further away than the fishing rod
        if  (lineRatioX(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y) || (lineRatioY(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y)) ) {

            if (!inventory_on) DrawCircleV((Vector2){ballPosition.x + lineRatioX(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y),

            ballPosition.y + CELL*SCALING/sqrt(3) + lineRatioY(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y) } , 2.5 * SCALING, RED);

            }

        // Render player circle behind of the rod
        if (ballPosition.y <= mousePos.y){
            DrawCircleV(ballPosition, 5*SCALING, LIME);
        }

        // Fishing rod - render if mouse is further away than the fishing rod
        if (!inventory_on) DrawLineEx((Vector2) { ballPosition.x,  ballPosition.y },
                
            (Vector2) { ballPosition.x + lineRatioX(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y),

            ballPosition.y + lineRatioY(mousePos.x,ballPosition.x,mousePos.y,ballPosition.y) } , 2 , DARKBROWN);

        // Render player circle in "front" of the rod
        if (ballPosition.y > mousePos.y){
            DrawCircleV(ballPosition, 5*SCALING, LIME);
        }

        // Draw player hat icon
        DrawTextureEx(HatTexture,(Vector2){ballPosition.x-CELL*1.8/4*SCALING-2,ballPosition.y-CELL*1.5},0,SCALING,WHITE); 
                       
        // inventory toggle key
        if (IsKeyPressed(KEY_I)) {
            inventory_on = !inventory_on; // flip
            PlaySound(blop);
            Optionsmenu = 0;
            Notebookmenu = 0;
        }

        // toggle inventory with mouse from the top right corner button
        inventory_on = mousePressInventory(mousePos, inventory_on);

        if (inventory_on != mousePressInventory(mousePos, inventory_on)){
            Optionsmenu = 0;
            PlaySound(blop);
            printf("(Input)\nPressed mouse on inventory!\n");
        }

        // toggle Options menu with mouse from the top right corner button
        Optionsmenu = mousePressOptions(mousePos, Optionsmenu);

        if (Optionsmenu != mousePressOptions(mousePos, Optionsmenu)){
            inventory_on = 0;
            Notebookmenu = 0;
            PlaySound(blop);
            printf("(Input)\nPressed mouse on Options!\n");
        }

        // Buttons for the top right corner options and inventory menus
        DrawTextureEx(InfoPic , (Vector2){screenWidth-120/2 * SCALING , 0} , 0, SCALING/2, tintColor(inventory_on));
        DrawTextureEx(InfoPic , (Vector2){screenWidth-200/2 * SCALING , 0} , 0, SCALING/2, tintColor2_Gray(Optionsmenu));
        
        DrawText("[I]nv", screenWidth-110/2 * SCALING , 1, 20, GOLD);
        DrawText("[O]pt", screenWidth-190/2 * SCALING , 1, 20, beige);
        // DrawTextEx(notebookFont, "[I]nfo", (Vector2){screenWidth-108/2*SCALING,1},20*SCALING/2,1,GOLD);

        // renderIsland(board,IslandCornerPic,IslandPic,rock);

        renderFoliage(board,xmastree,WeedPic);

        //Draw stuff if inventory is on
        if (inventory_on) {

            // Quit inventory menu outside edges
            if ((mousePos.x < screenWidth/8) || (mousePos.x > screenWidth-200) ||
                (mousePos.y > screenHeight-100) || (mousePos.y < screenHeight/8)) {
                    if(mousePos.y > 24) if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) inventory_on = 0;
                    
                }

            Notebookmenu = 0;
            // Picture for inventory background = Backbag
            DrawTextureEx(InventoryPic,(Vector2){ 0 , 0 }, 0 ,SCALING,WHITE);
            
            DrawTextEx(notebookFont, "Inventory", (Vector2){ screenWidth/2-100*SCALING, 80*SCALING}, 24*SCALING, 2, GOLD);

            DrawTextEx(notebookFont, "   [B] buy baits $5", (Vector2){ screenWidth/4-SIZE*2*SCALING, screenHeight/2-24*SCALING-SIZE*1*SCALING}, 24*SCALING, 2, GOLD);
            // DrawText(inventoryText, (screenWidth/4)*SCALING, (screenHeight/2+0+48*-1)*SCALING, 24*SCALING, GOLD);
            
            // Print texts of inventory with different colors
            for(int i = 0; i < 5; i++){
                int div = 4;
                int offset = 48;

                sprintf(inventoryText, "%s: %d",inventoryTexts[i],inventory.Total[i]) ;
                DrawText(inventoryText, (screenWidth/div-24), (screenHeight/4+48+offset*i)*SCALING, 24*SCALING, inventoryColors[i]);

            }

            // draw icons to inventory
            #ifndef ICONS_DEBUG
            DrawTextureEx(CoinsPic,(Vector2){screenWidth/4-2*SIZE,2*48+screenHeight/4*SCALING}, 0 ,SCALING,WHITE);
            DrawTextureEx(BaitsPic,(Vector2){screenWidth/4-2*SIZE,4*48+screenHeight/4*SCALING}, 0 ,SCALING,WHITE);
            DrawTextureEx(PerchesPic,(Vector2){screenWidth/4-3*SIZE,6*48+screenHeight/4*SCALING}, 0 ,SCALING,WHITE);
            DrawTextureEx(PikesPic,(Vector2){screenWidth/4-3*SIZE,8*48+screenHeight/4*SCALING}, 0 ,SCALING,WHITE);
            DrawTextureEx(BleaksPic,(Vector2){screenWidth/4-3*SIZE,10*48+screenHeight/4*SCALING}, 0 ,SCALING,WHITE);
            #endif

            // Info with the font for notebook and options
            DrawTextEx(notebookFont, OPAS, (Vector2){ screenWidth/2+24*2.5, screenHeight/2+12}, 24*SCALING, 2, GOLD);

            if( (mousePos.x > screenWidth/2+24*2.5) && (mousePos.x < screenWidth/2+300) &&
                (mousePos.y > screenHeight/2+24*3.5) && (mousePos.y < screenHeight/2+24*4+48)) {
                    // printf("%d Polttaa polttaa!\n\n",global.seconds);
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        Optionsmenu = 1;
                        inventory_on = 0;
                        PlaySound(blop);
                    }
                }

                if( (mousePos.x > screenWidth/2+24*2.5) && (mousePos.x < screenWidth/2+300) &&
                (mousePos.y > screenHeight/2+24*3.5*2) && (mousePos.y < screenHeight/2+24*4*2+24)) {
                    // printf("%d Polttaa polttaa!\n\n",global.seconds);
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        Optionsmenu = 0;
                        inventory_on = 0;
                        Notebookmenu = 1;
                        PlaySound(page);
                    }
                }

        }

        if (IsKeyPressed(KEY_N)) {
            Notebookmenu = !Notebookmenu;
            if (Optionsmenu || inventory_on){
                Optionsmenu = 0;
                inventory_on = 0;
                
            }
            if(Notebookmenu) PlaySound(page);

            
        }

        if(Notebookmenu){
            inventory_on = 0;
            Optionsmenu = 0;
            
            if( (mousePos.x > 200) && (mousePos.x < screenWidth) &&
            (mousePos.y > 24) && (mousePos.y < screenHeight/6)) {
                // printf("%d Polttaa polttaa!\n\n",global.seconds);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    Notebookmenu = 0;
                }
            }
            
            if( (mousePos.x > screenWidth-200) && (mousePos.x < screenWidth) &&
            (mousePos.y > 24) && (mousePos.y < screenHeight-200)) {
                // printf("%d Polttaa polttaa!\n\n",global.seconds);
                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    Notebookmenu = 0;
                }
            }

            DrawTexture(NotebookPic,0,0,WHITE);
            DrawTextEx(notebookFont, "Here is a placeholder\ntext for this game\n\nInstructions", (Vector2){ SIZE*12*SCALING/2, SIZE*8*SCALING/2}, 24*SCALING, 2, DARKGRAY);

            DrawTextEx(notebookFont, "You can move around\nusing either WASD\nor Arrow keys.\n  You can fish with the mouse\nor  space bar !", (Vector2){ SIZE*12*SCALING/2, SIZE*20*SCALING/2}, 24*SCALING, 2, DARKGRAY);
        }

        if (IsKeyPressed(KEY_O) || IsKeyPressed(KEY_ESCAPE)){
            Optionsmenu = !Optionsmenu;
            Notebookmenu = 0;
            PlaySound(blop);
        }

        if (Optionsmenu){
            if (IsKeyPressed(KEY_I)) {
                Optionsmenu = 0;
                
            }

            // Draw stuff if Options menu is open
            else {
                inventory_on = 0;
                DrawTextureEx(OptionsMenuPic,(Vector2){0,0,},0,1,WHITE);

                DrawTextEx(notebookFont, "----------------------------", (Vector2){ screenWidth/4, screenHeight/4}, 24*SCALING, 2, DARKGRAY);
                DrawTextEx(notebookFont, "\n\n\n\n----------------------------", (Vector2){ screenWidth/4, screenHeight/4}, 24*SCALING, 2, DARKGRAY);

                DrawTextEx(notebookFont, "\n\n[ SAVE ] to a file", (Vector2){ screenWidth/4, screenHeight/4}, 24*SCALING, 2, GOLD);

                DrawTextEx(notebookFont, OPTIONS, (Vector2){ screenWidth/4, screenHeight/2}, 24*SCALING, 2, beige);

                // Quit options menu outside edges
                if ((mousePos.x < screenWidth/8) || (mousePos.x > screenWidth-200) ||
                    (mousePos.y > screenHeight-100) || (mousePos.y < screenHeight/8)) {
                        if(mousePos.y > 24) if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) Optionsmenu = 0;
                        
                    }
                

                // Save record
                if ((mousePos.x > screenWidth/4) && (mousePos.x < screenWidth/2) &&
                    (mousePos.y > screenHeight/4) && (mousePos.y < screenHeight/3+screenHeight/10)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            save_records(inventory);
                            #ifdef DEBUG
                            printf("Saved the game file in the saves/ folder!\n\n");
                            #endif
                            PlaySound(blop);
                        }
                }
                
                // Check if pressed on check boxes
                if ((mousePos.x > screenWidth/4) && (mousePos.x < screenWidth/3) &&
                    (mousePos.y > screenHeight/2+1*SCALING) && (mousePos.y < screenHeight/2+24*SCALING)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            #ifdef DEBUG
                            printf("Pressed Sonar cheat!\n\n");
                            #endif
                            if (!(CheatCodes & SonarEnabled)) PlaySound(cheatSound);
                            else PlaySound(blop);
                            CheatCodes ^= SonarEnabled;
                        }
                }
                
                if ((mousePos.x > screenWidth/4) && (mousePos.x < screenWidth/3) &&
                    (mousePos.y > screenHeight/2+24*2*SCALING) && (mousePos.y < screenHeight/2+24*3*SCALING)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            #ifdef DEBUG
                            printf("Pressed Grid option!\n\n");
                            #endif
                            PlaySound(blop);
                            CheatCodes ^= GridEnabled;
                        }
                }              

                if ((mousePos.x > screenWidth/4) && (mousePos.x < screenWidth/3) &&
                    (mousePos.y > screenHeight/2+24*4*SCALING) && (mousePos.y < screenHeight/2+24*5*SCALING)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            #ifdef DEBUG
                            printf("Pressed Chaotic mode!\n\n");
                            #endif
                            PlaySound(blop);
                            CheatCodes ^= ChaoticEnabled;
                        }
                }                               

                // Close pressing on Exit game
                if ((mousePos.x > screenWidth/4) && (mousePos.x < screenWidth/3) &&
                    (mousePos.y > screenHeight/2+24*6*SCALING) && (mousePos.y < screenHeight/2+24*7*SCALING)) {
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            CloseWindow();
                            printf("Exit game!\n\n");
                        }
                }                                

                // Draw anchor icon for check boxes
                if (CheatCodes & SonarEnabled){
                    DrawTextureEx(AnchorPic,(Vector2){screenWidth/4+6,screenHeight/2+6*SCALING},0,1*SCALING,WHITE);
                }

                if (CheatCodes & GridEnabled){
                    DrawTextureEx(AnchorPic,(Vector2){screenWidth/4+6,screenHeight/2+14+24*2*SCALING},0,1*SCALING,WHITE);
                }

                if (CheatCodes & ChaoticEnabled){
                    DrawTextureEx(AnchorPic,(Vector2){screenWidth/4+6,screenHeight/2+18+24*4*SCALING},0,1*SCALING,WHITE);
                }
            }
        }

        if(IsKeyPressed(KEY_R)) landscape(board);
        
        // Draw Title texture alpha fade animation
        #ifndef DEBUG
        if ((TitleAlpha > 0) && (UpdateCounter.seconds > 3)) TitleAlpha -= GetFrameTime();
            if (TitleAlpha > 0) DrawTextureEx(TitleTexture, 
                // (Vector2){-255+TitleAlpha+screenWidth/4,24}, 
                (Vector2){screenWidth*0.25,SIZE*2},
                0, // no rotation
                TitleScaling(screenWidth), 
                (Color){255,255,255,TitleAlpha/2});
        #endif

        // Draw helpful information
        #ifdef DEBUG
        // FPS text
        int fps = GetFPS();
        char fpsText[16];
        sprintf(fpsText, "fps: %d", fps);
        if (GetFPS() < 60) DrawText(fpsText, 60, 0, 20, ORANGE);
        else DrawText(fpsText, 60, 0, 20, BLACK);

        // Duration
        sprintf(inventoryText, "duration: %2.d",global.seconds) ;
        DrawText(inventoryText, screenWidth/4, 0, 20, (Color){0,0,0,200});

        // print seconds
        sprintf(inventoryText, "Update: %d",30-UpdateCounter.seconds) ;
        DrawText(inventoryText, screenWidth/1.5, 0, 24, BLACK);
        
        // print cells amount
        if (CheatCodes & ChaoticEnabled) sprintf(inventoryText, "Cells: %3d  NO LIMIT (Chaotic)",countCells(board)) ;
        else sprintf(inventoryText, "Cells: %3d  Limit: 250",countCells(board));
        DrawText(inventoryText, screenWidth/4+120, screenHeight-SIZE/2, 24, BLACK);

        sprintf(inventoryText, "Clk speed: %.0f",CLOCKSPEED) ;
        DrawText(inventoryText, screenWidth/2, 0, 24, BLACK);
        
        // Debug banner
        DrawRectangle(screenWidth-80,screenHeight-30,screenWidth,screenHeight,(Color){255,255,255,100});
        DrawText("Debug", screenWidth-80, screenHeight-30, 24, DARKBLUE);

        #endif

        if (!inventory_on){ // Draw icons on the status bar below
            int countTotal = (inventory.Total[Perches]+inventory.Total[Pikes]+inventory.Total[Bleaks]);
            char Text[100] = "";

            DrawTextureEx(CoinsPic,(Vector2){42+SIZE/2,screenHeight-27},0, SCALING/2,WHITE);

            sprintf(Text,"%d",inventory.Total[Coins]);
            DrawText(Text,40*2+SIZE/2,screenHeight-27,24*SCALING/2,GOLD);

            DrawTextureEx(BaitsPic,(Vector2){40*3+SIZE/2,screenHeight-27}, 0 ,SCALING/2,WHITE);

            sprintf(Text,"%02d",inventory.Total[Baits]);
            DrawText(Text,40*4+3,screenHeight-27,24*SCALING/2,PINK);
        
            DrawTextureEx(PerchesPic,(Vector2){40*5,screenHeight-27}, 0 ,SCALING/2,WHITE);

            // Total fishes amount
 
            sprintf(Text,"%d total",countTotal);
            DrawText(Text,160*SCALING,screenHeight-SIZE/2,24*SCALING/2,BLACK);
        }

        // Buy baits by pressing B
        if (IsKeyPressed(KEY_B)) {
            if (inventory.Total[Coins] >= 5){
                inventory.Total[Coins] -= 5;
                inventory.Total[Baits] += 5;
            }
            else {
                #ifdef DEBUG
                printf("Not enough coins to buy baits! 5$ min, you have %d left \n\n",inventory.Total[Coins]);
                #endif
                PlaySound(robo);
            }
        }

        // update moude position, draw mouse cursor if DEBUG is not defined
        mousePos = GetMousePosition();

        // Tint for the entire screen
        #ifndef DEBUG
        DrawTextureEx(HookPic,mousePos,0,1.0f,WHITE);

        int targetTintAlpha = 64;

        DrawRectangle(0,0,screenWidth,screenHeight,(Color){40,20,60,targetTintAlpha});
        #endif
        
        EndDrawing();    
    }
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    CloseAudioDevice();    // Close audio system
    //--------------------------------------------------------------------------------------

    return 0;
}