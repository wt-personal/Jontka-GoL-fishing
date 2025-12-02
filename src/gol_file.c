/** 
 * @file gol_file.c
 * @author e2403299
 * @brief Gameplay functions for the game
 * @date 2025-11-30
 * @details This file includes Gameplay and GoL-based functions for the game.
*/

#include "header.h"
#include <stdio.h>
#include <raylib.h>
#include <math.h>

#include <stdlib.h>
#include <time.h>

/**
 * @brief Player movement - can be either WASD or arrow keys.
 * @param ballposx 
 * @param ballposy 
 * @param board 
 */
void playerMovement(float *ballposx, float *ballposy, struct cell board[SIZE][SIZE]){

    float dx = 0.0f, dy = 0.0f;
    if (IsKeyDown(EXITKEY)) CloseWindow();

    if (IsKeyDown(KEY_RIGHT) || (IsKeyDown(KEY_D))) dx += 1;
    if (IsKeyDown(KEY_LEFT)  || (IsKeyDown(KEY_A))) dx -= 1;
    if (IsKeyDown(KEY_UP)    || (IsKeyDown(KEY_W))) dy -= 1;
    if (IsKeyDown(KEY_DOWN)  || (IsKeyDown(KEY_S))) dy += 1;

    // Only move if there is input
    if (dx != 0 || dy != 0) {
        float len = sqrt(dx*dx + dy*dy);
        dx = dx / len * SPEED * GetFrameTime();
        dy = dy / len * SPEED * GetFrameTime();

        // Collision checking for the new positions
        float newX = *ballposx + dx;
        float newY = *ballposy + dy;

        // Convert player position into board cells
        float cellWidth =  (float) 800 / SIZE * SCALING;
        float cellHeight = (float) 600 / SIZE * SCALING;
        int cellX = (int)(newX / cellWidth);
        int cellY = (int)(newY / cellHeight);

        // Edges of the screen limit the movement
        if (cellX < 0) cellX = 0;
        if (cellX >= SIZE*SCALING) cellX = SIZE - 1;
        if (cellY < 0) cellY = 0;
        if (cellY >= SIZE*SCALING) cellY = SIZE - 1;

        // if landscape is detected, or weeds
        if ((board[cellY][cellX].landtype == 0) || (board[cellY][cellX].landtype == 7)) {

            // Reduce speeds inside weeds
            if (board[cellY][cellX].landtype == 7) {
                dx *= 0.4f; // reduce movement speed to 30%
                dy *= 0.4f;
            }

            // Move ball but stop at edges
            if (newX < 0)
                *ballposx = 0;
            else if (newX > screenWidth)
                *ballposx = screenWidth;
            else
                *ballposx += dx;

            if (newY < 0)
                *ballposy = 0;
            else if (newY > screenHeight)
                *ballposy = screenHeight;
            else
                *ballposy += dy;
        }
    }
}

/**
 * @brief Init glider pattern
 * @param board 
 * @param pop 
 */
void init_board_species(struct cell board[SIZE][SIZE],int pop){

    int species = pop - 1; // Calculate based on enum

    // Glider pattern
    board[2][3].current = species;
    board[3][4].current = species;
    board[4][2].current = species;
    board[4][3].current = species;
    board[4][4].current = species;
}

/**
 * @brief Random board (Chaotic)
 * @param board 
 * @param pop 
 */
void randomize_board(struct cell board[SIZE][SIZE],int pop){

    // const char *populationText[] = {"Empty","Perches","Pikes","Bleaks"};

    int species = pop - 1; // enum type
    int randomNum = 0;
    for(int j = 1; j < SIZE-1; j++){
        for(int i = 1; i < SIZE-1; i++){
            randomNum = rand() % 2;
            // Check if the cell is empty
            if (randomNum && (!board[j][i].current)) {
                board[j][i].current = species;
                //  if (i == SIZE-1) printf("Randomized cells: Chaotic mode\n\n");
            }
        }
    }
}

/**
 * @brief Scaling for the title
 * @param NewWidth 
 * @return 
 */
float TitleScaling(float NewWidth){
    NewWidth = (float) NewWidth / 800;
    return NewWidth;
}

/**
 * @brief Load boards from .txt files in the example_boards directory
 * @param board 
 * @param error 
 * @param choose_board 
 * @param pop 
 * @details This function was expanded from 
 */
void fileboard_example(struct cell board[SIZE][SIZE], char *error, unsigned int choose_board,int pop) {
    FILE *fptr;
    int ch;

    // Store filename into a string
    char filename[50] = "example_boards/example1.txt";

    // change pattern based on number
    sprintf(filename, "example_boards/example%d.txt", choose_board);  // convert integer into string
    
    #ifdef DEBUG // print info about the board loaded
    printf("\nRandom board: %d\nupdated from file: %s\n",choose_board,filename);
    printf("\nInitial setup,\n");
    #endif

    if ((choose_board >= 0) && (choose_board < 10)) fptr = fopen(filename, "r");

    else if (choose_board < 0){
        printf("Invalid board number\n");
        *error = 1;
        return;
    }

    if (fptr == NULL) {
        printf("Not able to open the file. Is the file located in the example_boards/ folder?\n");
        *error = 1;
        return;
    }

    // Random locations for the new patterns
    int randomX = rand() % 2 * (rand() % 20);
    int randomY = rand() % 2 * (rand() % 20);

    int row = 0, col = 0;
    while ((ch = fgetc(fptr)) != EOF) {
        if (ch == '0' || ch == '1') {
            if (row < SIZE && col < SIZE) {
                if(pop == Perches){
                    if((ch - '0') == 1) board[row+randomX][col+randomX].current = 1;
                    // else board[row][col].current = 0; // This will overwrite previous cells to zero
                }
                else if(pop == Pikes){
                    if((ch - '0') == 1) board[row+randomX][col+randomY].current = 2;
                    
                }
                else if(pop == Bleaks){
                    if((ch - '0') == 1) board[row+randomX][col+randomY].current = 3;
                    
                }
                
                else if(pop == Trouts){
                    if((ch - '0') == 1) board[row+randomX][col+randomY].current = 4;
                    
                }
                col++;
            }
            // if 1 put cell char, else put empty cell char
            ch == '1' ? printf("%c ",CELL_CHAR) : printf("%c ",EMPTY_CELL);

        } else if (ch == '\n') {
            row++;
            putchar(ch);
            col = 0;
        } else if (ch == '\r') {
            // Ignore carriage return Windows based character <cr> , dec 13 , hex 0D
        } else {
            putchar(ch);
            putchar(' ');
            *error = 2;
        }
    }
    #ifdef DEBUG
    if (*error == 2) printf("\n\nUnknown characters in the file, use only 1 and 0.");
    *error = 0; // reset error flag
    #endif

    fclose(fptr);
    printf("\n");
}

/**
 * @brief Uses trigonometry and Pythagorean theorem to calculate the fishing rod/float length and position X axis
 * @param mouseX 
 * @param playerX 
 * @param mouseY 
 * @param playerY 
 * @return cosine of fishing rod length
 */
float lineRatioX(float mouseX, float playerX, float mouseY, float playerY) {
    float div = sqrt(3);

    float size = 3;

    float dx = mouseX - playerX;
    float dy = mouseY - playerY;
    float distance = sqrtf( pow(dx,2) + pow(dy,2) );

    if (distance < CELL * SCALING * size / div) return 0;
    float angle = atan2f(dy, dx); 
    return cosf(angle) * CELL * SCALING * size / div;
}

/**
 * @brief Uses trigonometry and Pythagorean theorem to calculate the fishing rod/float length and position Y axis
 * @param mouseX 
 * @param playerX 
 * @param mouseY 
 * @param playerY 
 * @return sine of fishing rod length
 */
float lineRatioY(float mouseX, float playerX, float mouseY, float playerY) {
    float div = sqrt(3);

    float size = 3;

    float dx = mouseX - playerX;
    float dy = mouseY - playerY;
    float distance = sqrtf( pow(dx,2) + pow(dy,2) );

    if (distance < CELL * SCALING * size / div) return 0;
    float angle = atan2f(dy, dx);
    return sinf(angle) * CELL * SCALING * size / div;
}

/**
 * @brief Check if left mouse button is clicked inside the info box to open inventory
 * @param mousePos 
 * @param inventory_on 
 * @return value for the inventory_on
 */
int mousePressInventory(Vector2 mousePos, int inventory_on){
    // Check if mouse is inside an area of info box
    if ((mousePos.x >= screenWidth-(3*40) * SCALING/2)  && (mousePos.x <= (screenWidth-(40*SCALING/2)) && (mousePos.y <= 30 * SCALING/2))){
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                inventory_on = !inventory_on; // Flip the value

        } 
    }
    return inventory_on; 
}

/**
 * @brief Check if left mouse button is clicked inside the Options menu box [O]pt
 * @param mousePos 
 * @param Optionsmenu
 * @return value for the Optionsmenu
 */
int mousePressOptions(Vector2 mousePos, int Optionsmenu){
    // Check if mouse is inside an area of info box
    if ((mousePos.x >= screenWidth-(5*40) * SCALING/2)  && (mousePos.x <= (screenWidth-(3*40*SCALING/2)) && (mousePos.y <= 30 * SCALING/2))){
        
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Optionsmenu = !Optionsmenu; // Flip the value

        } 
    }
    return Optionsmenu; 
}

/**
 * @brief reset board with random zeros
 * @param board 
 */
void reset_board(struct cell board[SIZE][SIZE]){

    /*
        We know that the borders are landscape, so no cells can be at the borders.

        But clearing every cell is kept for future use of this function.
    */ 

    for(int j = 0; j < SIZE; j++){
        for(int i = 0; i < SIZE; i++){
            if (rand() % 2) board[i][j].current = 0;
            board[i][j].future = 0;
        }
    }
}

/**
 * @brief Check landscape and attempt to move cell to an empty cell etc.
 * @param x 
 * @param y 
 * @param board 
 */
void checkLandscape(int x, int y, struct cell board[SIZE][SIZE]) {

    // Check 3x3 coordinates for neighboring cells
    if (board[x][y].landtype != 0 && board[x][y].current != 0) {

        // If fish is inside weed, it can spread to nearby empty water cells
        if (board[x][y].landtype == 7) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (i == 0 && j == 0) continue;
                    int r = x + i;
                    int c = y + j;

                    // Check board edges
                    if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
                        // If neighboring cell is water and empty 
                        if (board[r][c].landtype == 0 && board[r][c].current == 0) {
                            // Random change of spawning new cell
                            if (rand() % 10 < 3) {
                                board[r][c].current = board[x][y].current;
                            }
                        }
                    }
                }
            }

            // return , does not continue into landscape checking
            return;
        }

        // Check for neighboring cells
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;
                int r = x + i;
                int c = y + j;

                // Check board limits
                if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
                    // Probability 30% - Find empty cell with no land and move to it instead
                    if (rand() % 10 < 3){
                        if (board[r][c].landtype == 0 && board[r][c].current == 0) {
                            board[r][c].current = board[x][y].current;
                            board[x][y].current = 0;
                            return;
                        }
                    }
                }
            }
        }

        // Fish ends up on dry land = remove
        board[x][y].current = 0;
    }
}

/**
 * @brief Sub function that counts neighbors for each species inside nextGen()
 * @param x 
 * @param y 
 * @param pop 
 * @param board 
 * @return number of neighboring cells
 */
int count_ne(int x, int y, int pop,struct cell board[SIZE][SIZE]) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int r = x + i;
			int c = y + j;
            
            if (r >= 0 && r < SIZE && c >= 0 && c < SIZE)
                if (board[r][c].current == pop)

                    count++;
        }
    }
    return count;
}

/**
 * @brief Board updates for cells and their generation
 * @param board 
 */
void nextGen(struct cell board[SIZE][SIZE]) {

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {

            // Count neighbors for all species
            int neighbors[5];

            for (int p = 1; p <= 4; p++){
                neighbors[p] = count_ne(i, j, p, board);
                }          

            // Check which population is currently at hand
            int pop = board[i][j].current;

            if (pop >= 1 && pop <= 4) {
                // Cell survives if it has 2 or 3 neighbors of the same population
                board[i][j].future = (neighbors[pop] == 2 || neighbors[pop] == 3) ? pop : 0;
            } else {
                // Empty cell: check for new births
                for (int p = 1; p <= 4; p++) {
                    if (neighbors[p] == 3) {
                        board[i][j].future = p;
                        break; // Only one new species per cell
                    }
                }
                // If no birth occurred, ensure cell stays empty
                if (board[i][j].future == 0)
                    board[i][j].future = 0;
            }
        }
    }

    // Update current generation
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j].current = board[i][j].future;

    // Check landscape interactions
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j].landtype != 0 && board[i][j].current != 0)
                checkLandscape(i, j, board);
}

/**
 * @brief Define landscape elements into the board.landtype
 * @param board 
 */
void landscape(struct cell board[SIZE][SIZE]){
    // first column left side
    for(int i = 0; i < SIZE; i++){
        board[i][0].landtype = (i % 2 + 1); // alternate between 1 and 2
    }
    // second column on the right side
    for(int i = 0; i < SIZE; i++){
        board[i][SIZE-1].landtype = (i % 2 + 1); // alternate between 1 and 2
    }

    // Top and bottom rows (status bars)
    for (int j = 1; j < SIZE - 1; j++) {
        if (j > 1 && j < SIZE - 2) {
            board[0][j].landtype = 3;              // top row
            board[SIZE - 1][j].landtype = 3;       // bottom row
        } else {
            board[0][j].landtype = 4;              // top row corners
            board[SIZE - 1][j].landtype = 4;       // bottom row corners
        }
    }

    // make island 
    board[10][10].landtype = 5;
    board[10][11].landtype = 5;
    board[10][12].landtype = 5;

    // Position for the weeds, kaislikko
    board[4][5].landtype = 7;
    board[4][6].landtype = 7;
    board[4][7].landtype = 7;
    board[4][8].landtype = 7;

    board[5][5].landtype = 7;
    board[5][6].landtype = 7;
    board[5][7].landtype = 7;
    board[5][8].landtype = 7;

    // rock next to the first island
    board[10][14].landtype = 6;

    // random rocks
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if (rand() % 100 < 3 && board[j][i].landtype == 0) board[j][i].landtype = 6;
        }
    }

}

/**
 * @brief Fishing mechanism: - Check the float’s position relative to the cell and whether the player clicks the mouse button
 * @param Floatpos 
 * @param board 
 * @param erase 
 * @param cheats
 * @return number value for the type of fish that was redeemed by succesful fishing
 */
int CheckFishingArea(Vector2 Floatpos, struct cell board[SIZE][SIZE],int erase, int cheats) {
    #ifdef PRINT_FLOAT_POS
    printf("Float - Pos X: %f\n", Floatpos.x);
    printf("Float - Pos Y: %f\n\n", Floatpos.y);
    #endif

    int RedeemFish = 0;

    float cellWidth  = (float)screenWidth / SIZE;
    float cellHeight = (float)screenHeight / SIZE;

    for (int j = 0; j < SIZE; j++) {
        for (int i = 0; i < SIZE; i++) {
            if (board[j][i].current) {
                float cellX = i * cellWidth;
                float cellY = j * cellHeight;

                // Check if the float overlaps this cell
                if (Floatpos.x >= cellX &&
                    Floatpos.x < cellX + cellWidth &&
                    Floatpos.y >= cellY &&
                    Floatpos.y < cellY + cellHeight)
                {

                    
                    RedeemFish = board[j][i].current;

                    // Print pink square if cheats are enabled
                    if (cheats & SonarEnabled){
                        if (RedeemFish > 0) DrawRectangle(cellX, cellY, cellWidth, cellHeight, PINK);
                    }
                    
                    

                    // Check for mouse click while the float is over this cell
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
                        // remove fish if player has baits (erase defined)
                        if (erase) board[j][i].current = 0;  // remove fish from cell
                        return RedeemFish;        
                    }
                return 0;
                }
            }
        }
    }

    // No cell
    return 0;
}

/**
 * @brief This function calculates total amount of cells
 * @param board 
 * @return amount of living cells
 */
int countCells (struct cell board[SIZE][SIZE]){
    int amount = 0;
    for(int i = 1; i < SIZE-1; i++){
        for(int j = 1; j < SIZE-1; j++){
            if (board[i][j].current) amount++;
        }
    }
    return amount;
}

/**
 * @brief Save a record file of inventory into saves/ folder
 * @param inventory 
 */
void save_records(struct noteBook inventory){

    // Calculate total amount of fish
    int countTotal = (inventory.Total[Perches]+inventory.Total[Pikes]+inventory.Total[Bleaks]+inventory.Total[Trouts]);

    // write to a file
    FILE *filepointer;
    filepointer = fopen("saves/record.txt", "w");

    if (!filepointer) {
        printf("Error opening the file!\n\n");
        return;
    }

    // Write records to a text file
    fprintf(filepointer, "--- Fishing records ---\n\n");

    fprintf(filepointer, "Coins:   %2d\n",inventory.Total[Coins]);

    fprintf(filepointer, "Baits:   %2d\n",inventory.Total[Baits]);
    fprintf(filepointer, "\n--- Fishes          ---\n");

    fprintf(filepointer, "Perches: %2d\n",inventory.Total[Perches]);

    fprintf(filepointer, "Pikes:   %2d\n",inventory.Total[Pikes]);

    fprintf(filepointer, "Bleaks:  %2d\n",inventory.Total[Bleaks]);    

    fprintf(filepointer, "Trouts:  %2d\n",inventory.Total[Trouts]);    

    fprintf(filepointer, "\nTotal amount of fish =  %d\n",countTotal);    

    // Close the file
    fclose(filepointer);
}