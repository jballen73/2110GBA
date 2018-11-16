#include "graphics.h"
#include "gba.h"
#include "images/sprites.h"
#include "images/bossScreen.h"
#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
// TA-TODO: Include any header files for images generated by nin10kit.
// Example for the provided garbage image:
//#include "images/garbage.h"
static void drawSprites(void);
char* itoa(int, char* , int);
volatile OamEntry shadow[128];
volatile OamEntry* playerCharacter = &shadow[0];
volatile OamEntry* playerShot0 = &shadow[1];
volatile OamEntry* playerShot1 = &shadow[2];
volatile OamEntry* playerShot2 = &shadow[3];
volatile OamEntry* playerShot3 = &shadow[4];
volatile OamEntry* deathCountText = &shadow[5];
volatile OamEntry* savedText = &shadow[6];
volatile OamEntry* checkpointText = &shadow[7];
volatile OamEntry* sonicBoss = &shadow[8];
void hideSprites(void) {
    for(int i = 0; i < 128; i++) {
        shadow[i].attr0 = ATTR0_HIDE;
    }
    drawSprites();
}
void graphicsInit(void) {
    DMA[3].src = sprites_palette;
    DMA[3].dst = SPRITEPAL;
    DMA[3].cnt = SPRITES_PALETTE_LENGTH | DMA_ON;

    DMA[3].src = sprites;
    DMA[3].dst = &CHARBLOCKBASE[5];
    DMA[3].cnt = SPRITES_LENGTH | DMA_ON;


    for(int i = 0; i < 128; i++) {
        shadow[i].attr0 = ATTR0_HIDE;
    }
    playerCharacter = &shadow[0];
    // playerCharacter->attr0 = 50 | SPRITES_PALETTE_TYPE | PLAYERCHARACTERSPRITE_SPRITE_SHAPE;
    // playerCharacter->attr1 = 50 | PLAYERCHARACTERSPRITE_SPRITE_SIZE;
    playerCharacter->attr2 = PLAYERCHARACTERSPRITE_PALETTE_ID | PLAYERCHARACTERSPRITE_ID;

    playerShot0 = &shadow[1];
    playerShot0->attr2 = PLAYERSHOT_PALETTE_ID | PLAYERSHOT_ID;

    playerShot1 = &shadow[2];
    playerShot1->attr2 = PLAYERSHOT_PALETTE_ID | PLAYERSHOT_ID;

    playerShot2 = &shadow[3];
    playerShot2->attr2 = PLAYERSHOT_PALETTE_ID | PLAYERSHOT_ID;

    playerShot3 = &shadow[4];
    playerShot3->attr2 = PLAYERSHOT_PALETTE_ID | PLAYERSHOT_ID;

    deathCountText = &shadow[5];
    deathCountText->attr2 = DEATHCOUNTTEXT_PALETTE_ID | DEATHCOUNTTEXT_ID;

    savedText = &shadow[6];
    savedText->attr2 = SAVEDTEXT_PALETTE_ID | SAVEDTEXT_ID;

    checkpointText = &shadow[7];
    checkpointText->attr2 = CHECKPOINTTEXT_PALETTE_ID | CHECKPOINTTEXT_ID;

    sonicBoss = &shadow[8];
    sonicBoss->attr2 = SONICSPRITES0_PALETTE_ID | SONICSPRITES0_ID;
}
// TA-TODO: Add any draw/undraw functions for sub-elements of your app here.
// For example, for a snake game, you could have a drawSnake function
// or a drawFood function
//
// e.g.:
// static void drawSnake(Snake* snake);
// static void drawFood(Food* food);

static void drawDeathCount(void) {
    deathCountText->attr0 = 8 | SPRITES_PALETTE_TYPE | DEATHCOUNTTEXT_SPRITE_SHAPE;
    deathCountText->attr1 = 185 | DEATHCOUNTTEXT_SPRITE_SIZE;
}
static void drawSaveText(int on) {
    if (on > 0) {
        savedText->attr0 = 5 | SPRITES_PALETTE_TYPE | SAVEDTEXT_SPRITE_SHAPE;
        savedText->attr1 = 10 | SAVEDTEXT_SPRITE_SIZE;
    } else {
        savedText->attr0 = ATTR0_HIDE;
    }
}
static void drawCheckPointText(int on) {
    if (on > 0) {
        checkpointText->attr0 = 5 | SPRITES_PALETTE_TYPE | CHECKPOINTTEXT_SPRITE_SHAPE;
        checkpointText->attr1 = 10 | CHECKPOINTTEXT_SPRITE_SIZE;
    } else {
        checkpointText->attr0 = ATTR0_HIDE;
    }
}
static void drawShot0(int xpos, int ypos, int direction) {
    playerShot0->attr0 = (ypos&0x00FF) | SPRITES_PALETTE_TYPE | PLAYERSHOT_SPRITE_SHAPE;
    if (direction){
        playerShot0->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE; 
    } else {
        playerShot0->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE | ATTR1_HFLIP; 
    }
}
static void drawShot1(int xpos, int ypos, int direction) {
    playerShot1->attr0 = (ypos&0x00FF) | SPRITES_PALETTE_TYPE | PLAYERSHOT_SPRITE_SHAPE;
    if (direction){
        playerShot1->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE; 
    } else {
        playerShot1->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE | ATTR1_HFLIP; 
    }
}
static void drawShot2(int xpos, int ypos, int direction) {
    playerShot2->attr0 = (ypos&0x00FF) | SPRITES_PALETTE_TYPE | PLAYERSHOT_SPRITE_SHAPE;
    if (direction){
        playerShot2->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE; 
    } else {
        playerShot2->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE | ATTR1_HFLIP; 
    }
}
static void drawShot3(int xpos, int ypos, int direction) {
    playerShot3->attr0 = (ypos&0x00FF) | SPRITES_PALETTE_TYPE | PLAYERSHOT_SPRITE_SHAPE;
    if (direction){
        playerShot3->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE; 
    } else {
        playerShot3->attr1 = (xpos&0x00FF) | PLAYERSHOT_SPRITE_SIZE | ATTR1_HFLIP; 
    }
}
static void drawPlayer(int xpos, int ypos) {
    playerCharacter->attr0 = (ypos&0x00FF) | SPRITES_PALETTE_TYPE | PLAYERCHARACTERSPRITE_SPRITE_SHAPE;
    playerCharacter->attr1 = (xpos&0x01FF) | PLAYERCHARACTERSPRITE_SPRITE_SIZE;
    
}
static void drawBoss(Boss *boss) {
    sonicBoss->attr0 = (boss->ypos & 0x00FF) | SPRITES_PALETTE_TYPE | SONICSPRITES_SPRITE_SHAPE;
    sonicBoss->attr1 = (boss->xpos & 0x00FF) | SONICSPRITES_SPRITE_SIZE | (!boss->direction ? ATTR1_HFLIP : 0);
    if (boss->spriteNum) {
        sonicBoss->attr2 = SONICSPRITES1_PALETTE_ID | SONICSPRITES1_ID;
    } else {
        sonicBoss->attr2 = SONICSPRITES0_PALETTE_ID | SONICSPRITES0_ID;
    }
}
static void drawBossHealthBar(Boss *boss) {
    drawRectDMA(0, 0, 240, 10, WHITE);
    if (boss->health > 0) {
        drawRectDMA(0, 0, boss->health * 4, 10, MAGENTA);
    }
}
static void drawSprites(void) {
    DMA[3].src = shadow;
    DMA[3].dst = OAMMEM;
    DMA[3].cnt = 128*4 | DMA_ON;
}
// This function will be used to draw everything about the app
// including the background and whatnot.
void fullDrawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    drawFullScreenImageDMA(state->room->backgroundImage);
}

// This function will be used to undraw (i.e. erase) things that might
// move in a frame. E.g. in a Snake game, erase the Snake, the food & the score.
void undrawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    UNUSED(state);
}

// This function will be used to draw things that might have moved in a frame.
// For example, in a Snake game, draw the snake, the food, the score.
void drawAppState(AppState *state) {
    // TA-TODO: IMPLEMENT.
    //if (state->levelChange > 0) {

       // state->levelChange--;
    //}
    drawPlayer(state->thePlayerCharacter->xpos, state->thePlayerCharacter->ypos);
    
    drawShot0(state->shot0->xpos, state->shot0->ypos, state->shot0->direction);
    drawShot1(state->shot1->xpos, state->shot1->ypos, state->shot1->direction);
    drawShot2(state->shot2->xpos, state->shot2->ypos, state->shot2->direction);
    drawShot3(state->shot3->xpos, state->shot3->ypos, state->shot3->direction);
    drawDeathCount();
    drawSaveText(state->toSave);
    drawCheckPointText(state->checkpoint);
    drawSprites();
    //drawFullScreenImageDMA(state->room->backgroundImage);
    //drawCenteredString(170, 0, 50, 20,  "Deaths: ", BLACK);
   
    char deathString[16];
    itoa(state->deathCount, deathString, 10);
    drawCenteredString(200, 4, 50, 20, deathString, BLACK);
}
void fullDrawBossAppState(AppState *state) {
    drawFullScreenImageDMA(bossScreen);
    drawBossAppState(state);
}
void drawBossAppState(AppState *state) {
    drawPlayer(state->thePlayerCharacter->xpos, state->thePlayerCharacter->ypos);
    
    drawShot0(state->shot0->xpos, state->shot0->ypos, state->shot0->direction);
    drawShot1(state->shot1->xpos, state->shot1->ypos, state->shot1->direction);
    drawShot2(state->shot2->xpos, state->shot2->ypos, state->shot2->direction);
    drawShot3(state->shot3->xpos, state->shot3->ypos, state->shot3->direction);
    drawBoss(state->boss);
    drawSprites();
    drawBossHealthBar(state->boss);
}

