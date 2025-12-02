/** 
 * @file render.c
 * @author e2403299
 * @brief Rendering functions for the gol v2
 * @date 2025-11-30
 * @details This file includes functions related to rendering textures in the game.
*/

#include "header.h"

#include <stdio.h>
#include <raylib.h>
#include <math.h>

// setup random numbers and seed
#include <stdlib.h>
#include <time.h>

/**
 * @brief render foliage (Trees and weed) on the center of screen
 * @param board 
 * @param xmastree 
 * @param weed 
 */

void renderFoliage(struct cell board[SIZE][SIZE], Texture2D xmastree, Texture2D weed) {
    float cellWidth = (800.0f / SIZE) * SCALING;
    float cellHeight = (600.0f / SIZE) * SCALING;


    if(board[10][10].landtype == 5) DrawTextureEx(xmastree, (Vector2){screenWidth/4+cellWidth, screenHeight/4-cellHeight*2},0,1,WHITE);

    // Draw weed
    for(int j = 0; j < SIZE; j++){
        for(int i = 0; i < SIZE; i++){
            if(board[j][i].landtype == 7) DrawTextureEx(weed, (Vector2){cellWidth*i, cellHeight*j*SCALING/2},0,SCALING/2, (Color){125,255,255,200}); // Weed
        }
    }
}

/**
 * @brief render lanscape renders the game borders
 * @param board 
 * @param ShorePic 
 * @param StatusBar 
 * @param StatusBarCorner 
 */
void renderLandscape(struct cell board[SIZE][SIZE], Texture2D ShorePic, Texture2D StatusBar, Texture2D StatusBarCorner) {

    // First column on the left
    for (int i = 0; i < SIZE; i++) {

        bool flipVertical = (i % 2 == 1);

        Rectangle src = {
            0,
            0,
            40,
            flipVertical ? -30 : 30   // vertical flip every other tile
        };

        Rectangle dest = {
            0,
            24 * i,
            32,
            24                 
        };

        DrawTexturePro(ShorePic, src, dest, (Vector2){0,0}, 0, WHITE);
    }

    // Second column on the right
    for (int i = 0; i < SIZE; i++) {

        bool flipVertical = (i % 2 == 1);

        Rectangle src = {
            0,
            0,
            -40,
            flipVertical ? -30 : 30   // vertical flip every other tile
        };

        Rectangle dest = {
            screenWidth-32,
            24 * i,
            32,
            24                 
        };

        DrawTexturePro(ShorePic, src, dest, (Vector2){0,0}, 0, WHITE);
    }

    // Top row for status bar
    for (int i = 1; i < SIZE-2; i++) {

        Rectangle src = {
            0,
            0,
            i == SIZE-1 ? -40 : 40,
            30   
        };

        Rectangle dest = {
            32 * i  ,
            0,
            32,
            24
        };

        if (i == 1) DrawTexturePro(StatusBarCorner, src, dest, (Vector2){0,0}, 0, WHITE);
        else DrawTexturePro(StatusBar, src, dest, (Vector2){0,0}, 0, WHITE);
    }

    // Bottom row for status bar
    for (int i = 1; i < SIZE-1; i++) {

        Rectangle src = {
            0,
            0,
            i == SIZE-1 ? -40 : 40,
            -30   
        };

        Rectangle dest = {
            32 * i  ,
            screenHeight-24,
            32,
            24
        };

        if (i == 1 || i == SIZE-1) DrawTexturePro(StatusBarCorner, src, dest, (Vector2){0,0}, 0, WHITE);
        else DrawTexturePro(StatusBar, src, dest, (Vector2){0,0}, 0, WHITE);
    }
}

/**
 * @brief render island and rocks
 * @param board 
 * @param Islandcorner 
 * @param Islandpic 
 * @param rock 
 */
void renderIsland(struct cell board[SIZE][SIZE], Texture2D Islandcorner, Texture2D Islandpic,Texture2D rock) {

    float cellWidth = (800.0f / SIZE) * SCALING;
    float cellHeight = (600.0f / SIZE) * SCALING;

    // render rocks
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(board[j][i].landtype == 6) DrawTextureEx(rock, (Vector2){cellWidth*i, cellHeight*j},0,SCALING/2,WHITE);
        }
    }

    // draw two island textures
    if(board[10][10].landtype == 5) DrawTextureEx(Islandpic, (Vector2){10*cellWidth, 10*cellHeight},0,SCALING/2, WHITE); // Draw corner

    Rectangle src = { 0, 0, -Islandcorner.width, Islandcorner.height };  // negative width flips it
    Rectangle dest = { 12*cellWidth, 10*cellHeight, Islandcorner.width * 0.5f*SCALING, Islandcorner.height * 0.5f*SCALING };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(Islandcorner, src, dest, origin, 0, WHITE);

    if(board[10][15].landtype == 5) DrawTextureEx(rock, (Vector2){screenWidth/2+cellWidth*4,screenHeight/2},0, SCALING/2, WHITE);

}

/**
 * @brief print_board() renders squares of the fish and other visuals based on Options
 * @param board 
 * @param Cheatcodes 
 */
void render_board(struct cell board[SIZE][SIZE],int Cheatcodes){

    Color gridColor = {0,0,20,20};  

    float cellWidth = 800 / SIZE * SCALING;
    float cellHeight = 600 / SIZE * SCALING;

    Color BleakColor = {0x3c,0x34,0x4a, 125}; // Purple tint

    if (Cheatcodes){
        for (int j = 0; j < SIZE; j++) {
            for (int i = 0; i < SIZE; i++) {
                // Draw rectangles if Sonar is enabled
                if(Cheatcodes & SonarEnabled){
                    if (board[j][i].current == 1) DrawRectangle(i * cellWidth, j * cellHeight, cellWidth, cellHeight, DARKGREEN);
                    else if (board[j][i].current == 2) DrawRectangle(i * cellWidth, j * cellHeight, cellWidth, cellHeight, GREEN);
                    else if (board[j][i].current == 3) DrawRectangle(i * cellWidth, j * cellHeight, cellWidth, cellHeight, BleakColor);
                    else if (board[j][i].current == 4) DrawRectangle(i * cellWidth, j * cellHeight, cellWidth, cellHeight, GOLD);
                }


                // check if second bit is set
                if (Cheatcodes & GridEnabled) {
                    DrawRectangleLines(i * cellWidth, j * cellHeight, cellWidth, cellHeight, gridColor); // Grid visual
                }
                
            }
        }
    }
}

/**
 * @brief Tint color for inventory button 
 * @param inventory_on 
 * @return color tint
 */
Color tintColor(int inventory_on) {
    return inventory_on ? (Color) DARKGREEN : LIME;
}

/**
 * @brief Tint color for options button 
 * @param inventory_on 
 * @return gray tint
 */
Color tintColor2_Gray(int inventory_on) {
    return inventory_on ? (Color) DARKGRAY : GRAY;
}