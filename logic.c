#include "logic.h"
#include "graphics.h"
#include "gba.h"
#include "images/basicScreen.h"
#include "images/jumpScreen.h"
#include "images/tallObstacleScreen.h"
#include "images/saveScreen.h"
#include "images/saveJumpScreen.h"
#include "images/longJumpScreen.h"
#include "images/spikeScreen.h"
#include "images/saveLaddering.h"
#include "images/longReverseJumpScreen.h"
#include "images/tightSpikeScreen.h"
#include <stdlib.h>
//extern volatile OamEntry* shadow;
static Room **gameRooms;
static void makeSave(AppState* currentState, AppState* nextState);
void initializeAppState(AppState* appState) {
    // TA-TODO: Initialize everything that's part of this AppState struct here.
    // Suppose the struct contains random values, make sure everything gets
    // the value it should have when the app begins.
    gameRooms = malloc(NUM_ROOMS * sizeof(Room));
    Room* room0 = malloc(sizeof(Room));
    room0->backgroundImage = basicScreen;
    room0->collisionMap = basicScreenCollision;
    gameRooms[0] = room0;
    Room* room1 = malloc(sizeof(Room));
    room1->backgroundImage = jumpScreen;
    room1->collisionMap = jumpScreenCollision;
    gameRooms[1] = room1;
    Room* room2 = malloc(sizeof(Room));
    room2->backgroundImage = tallObstacleScreen;
    room2->collisionMap = tallObstacleScreenCollision;
    gameRooms[2] = room2;
    Room* room3 = malloc(sizeof(Room));
    room3->backgroundImage = saveScreen;
    room3->collisionMap = saveScreenCollision;
    gameRooms[3] = room3;
    Room* room4 = malloc(sizeof(Room));
    room4->backgroundImage = saveJumpScreen;
    room4->collisionMap = saveJumpScreenCollision;
    gameRooms[4] = room4;
    Room* room5 = malloc(sizeof(Room));
    room5->backgroundImage = longJumpScreen;
    room5->collisionMap = longJumpScreenCollision;
    gameRooms[5] = room5;
    Room* room6 = malloc(sizeof(Room));
    room6->backgroundImage = spikeScreen;
    room6->collisionMap = spikeScreenCollision;
    gameRooms[6] = room6;
    Room* room7 = malloc(sizeof(Room));
    room7->backgroundImage = saveLaddering;
    room7->collisionMap = saveLadderingCollision;
    gameRooms[7] = room7;
    Room* room8 = malloc(sizeof(Room));
    room8->backgroundImage = longReverseJumpScreen;
    room8->collisionMap = longReverseJumpScreenCollision;
    gameRooms[8] = room8;
    Room* room9 = malloc(sizeof(Room));
    room9->backgroundImage = tightSpikeScreen;
    room9->collisionMap = tightSpikeScreenCollision;
    gameRooms[9] = room9;

    Character *newPlayerCharacter =  (Character*)malloc(sizeof(Character));
    newPlayerCharacter->xvel = 0;
    newPlayerCharacter->yvel = 0;
    newPlayerCharacter->xpos = 2;
    newPlayerCharacter->ypos = 100;
    newPlayerCharacter->doubleJump = 1;
    newPlayerCharacter->airFrames = 0;
    newPlayerCharacter->direction = 1;

    Shot *newShot0 = (Shot*)malloc(sizeof(Shot));
    newShot0->xpos = WIDTH;
    newShot0->ypos = HEIGHT;
    newShot0->inUse = 0;
    newShot0->direction = 1;

    Shot *newShot1 = (Shot*)malloc(sizeof(Shot));
    newShot1->xpos = WIDTH;
    newShot1->ypos = HEIGHT;
    newShot1->inUse = 0;
    newShot1->direction = 1;

    Shot *newShot2 = (Shot*)malloc(sizeof(Shot));
    newShot2->xpos = WIDTH;
    newShot2->ypos = HEIGHT;
    newShot2->inUse = 0;
    newShot2->direction = 1;

    Shot *newShot3 = (Shot*)malloc(sizeof(Shot));
    newShot3->xpos = WIDTH;
    newShot3->ypos = HEIGHT;
    newShot3->inUse = 0;
    newShot3->direction = 1;

    CurrentSave *initialSave = (CurrentSave*)malloc(sizeof(CurrentSave));
    initialSave->yvel = 0;
    initialSave->xpos = 2;
    initialSave->ypos = 100;
    initialSave->direction = 1;
    initialSave->airFrames = 0;
    initialSave->roomNum = STARTING_ROOM;

    CurrentSave *initialCheckpointSave = (CurrentSave*)malloc(sizeof(CurrentSave));
    initialCheckpointSave->yvel = 0;
    initialCheckpointSave->xpos = 2;
    initialCheckpointSave->ypos = 100;
    initialCheckpointSave->direction = 1;
    initialCheckpointSave->airFrames = 0;
    initialCheckpointSave->roomNum = STARTING_ROOM;

    appState->thePlayerCharacter = newPlayerCharacter;
    appState->shot0 = newShot0;
    appState->shot1 = newShot1;
    appState->shot2 = newShot2;
    appState->shot3 = newShot3;
    appState->roomNum = STARTING_ROOM;
    appState->room = gameRooms[appState->roomNum];
    appState->gameOver = 0;
    appState->deathCount = 0;
    appState->levelChange = 0;
    appState->toSave = 0;
    appState->currentSave = initialSave;
    appState->checkpointSave = initialCheckpointSave;
}

// TA-TODO: Add any process functions for sub-elements of your app here.
// For example, for a snake game, you could have a processSnake function
// or a createRandomFood function or a processFoods function.
//
// e.g.:
// static Snake processSnake(Snake* currentSnake);
// static void generateRandomFoods(AppState* currentAppState, AppState* nextAppState);
static Shot* getAvailableShot(AppState* state, AppState* nextState) {
    if (!state->shot0->inUse) {
        return nextState->shot0;
    } else if (!state->shot1->inUse) {
        return nextState->shot1;
    } else if (!state->shot2->inUse) {
        return nextState->shot2;
    } else if (!state->shot3->inUse) {
        return nextState->shot3;
    } else {
        return NULL;
    }
}

static u16 getBackgroundPixel(AppState *state, int xpos, int ypos) {
    return state->room->collisionMap[OFFSET(ypos, xpos, WIDTH)];
}

static u16 checkGroundCollision(AppState *state) {
    u16 result = 0;
    int collisionArray[16] = {14,14,14,16,16,16,14,14,14,14,16,16,16,14,14,14};
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + i,
        state->thePlayerCharacter->ypos + collisionArray[i]);
        result = result | (pixel == 0x0000) | (pixel == SAVE_BLOCK);
        result = result | ((pixel == GROUND_KILL_VALUE) <<4);
        
    }
    return result;
}
static int checkTopCollision(AppState *state) {
    int result = 0;
    int collisionArray[16] = {12, 11, 9, 7, 5, 3, 1, -1, -1, 1, 3, 5, 7, 9, 11, 12};
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + i, 
        state->thePlayerCharacter->ypos + collisionArray[i]);
        result = result | (pixel == 0x0000) | (pixel == SAVE_BLOCK);
        result = result | ((pixel == GROUND_KILL_VALUE) <<4); 
    }
    return result;
}
static int checkLeftCollision(AppState *state) {
    int result = 0;
    int collisionArray[16] = {6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, -1, 3, 2};
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + collisionArray[i],
        state->thePlayerCharacter->ypos + i);
        result = result | (pixel == 0x0000) | ((pixel == GROUND_KILL_VALUE) << 4)
         | ((pixel == WALL_BACK) << 2) | (pixel == SAVE_BLOCK);
        
    }
    return result;
}
static int checkRightCollision(AppState *state) {
    int result = 0;
    int collisionArray[16] = {9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 12, 13};
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + collisionArray[i],
        state->thePlayerCharacter->ypos + i);
        result = result | (pixel == 0x0000) | ((pixel == GROUND_KILL_VALUE) << 4) 
        | ((pixel == WALL_ADVANCE) << 3) | (pixel == SAVE_BLOCK);
        
    }
    return result;
}

static int checkShotCollision(AppState *state, Shot *shot) {
    int result = 0;
    int xpos;
    if (shot->direction) {
        xpos = shot->xpos + 8;
    } else {
        xpos = shot->xpos - 1;
    }
    if (xpos < WIDTH && xpos > 0) {
        for (int i = 0; i < 8; i++) {
            u16 pixel = getBackgroundPixel(state, xpos, shot->ypos + i);
            result = result | (pixel == 0x0000) | ((pixel == SAVE_BLOCK) << 5)
             | ((pixel == GROUND_KILL_VALUE) << 4);
        }
    }
    return result;
}

static void updateShot(AppState* currentAppState, AppState* nextAppState, Shot* currStateShot, Shot* nextStateShot) {
    if (currStateShot->inUse) {
        if (currStateShot->xpos >= WIDTH || currStateShot->xpos < 0) {
            nextStateShot->inUse = 0;
            nextStateShot->xpos = WIDTH;
        } else {
            if (currStateShot->direction) {
                nextStateShot->xpos = currStateShot->xpos + 2;
            } else {
                nextStateShot->xpos = currStateShot->xpos - 2;
            }
        }
        int collisions = checkShotCollision(currentAppState, currStateShot);
        if (collisions & COLLISION_SAVE) {
            makeSave(currentAppState, nextAppState);
            nextStateShot->inUse = 0;
            nextStateShot->xpos = WIDTH;

        } else if (collisions & GROUND_REGULAR || collisions & GROUND_KILL) {
            nextStateShot->inUse = 0;
            nextStateShot->xpos = WIDTH;
        }
    }
}

static void clearAllShots(AppState* nextState) {
    nextState->shot0->inUse = 0;
    nextState->shot0->xpos = WIDTH;
    nextState->shot1->inUse = 0;
    nextState->shot1->xpos = WIDTH;
    nextState->shot2->inUse = 0;
    nextState->shot2->xpos = WIDTH;
    nextState->shot3->inUse = 0;
    nextState->shot3->xpos = WIDTH;
}

static void makeSave(AppState* currentState, AppState* nextState) {
    nextState->currentSave->yvel = currentState->thePlayerCharacter->yvel;
    nextState->currentSave->xpos = currentState->thePlayerCharacter->xpos;
    nextState->currentSave->ypos = currentState->thePlayerCharacter->ypos;
    nextState->currentSave->direction = currentState->thePlayerCharacter->direction;
    nextState->currentSave->airFrames = currentState->thePlayerCharacter->airFrames;
    nextState->currentSave->roomNum = currentState->roomNum;
    nextState->toSave = 20;
}

static void makeCheckpoint(AppState* nextState) {
    nextState->checkpointSave->xpos = 2;
    nextState->checkpointSave->ypos = 122;
    nextState->checkpointSave->roomNum = nextState->roomNum;
    nextState->checkpoint = 20;
}
// This function processes your current app state and returns the new (i.e. next)
// state of your application.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {
    /* TA-TODO: Do all of your app processing here. This function gets called
     * every frame.
     *
     * To check for key presses, use the KEY_JUST_PRESSED macro for cases where
     * you want to detect each key press once, or the KEY_DOWN macro for checking
     * if a button is still down.
     *
     * To count time, suppose that the GameBoy runs at a fixed FPS (60fps) and
     * that vblank is processed once per frame. Use the vBlankCounter variable
     * and the modulus % operator to do things once every (n) frames. Note that
     * you want to process button every frame regardless (otherwise you will
     * miss inputs.)
     *
     * Do not do any drawing here.
     *
     * TA-TODO: VERY IMPORTANT! READ THIS PART.
     * You need to perform all calculations on the currentAppState passed to you,
     * and perform all state updates on the nextAppState state which we define below
     * and return at the end of the function. YOU SHOULD NOT MODIFY THE CURRENTSTATE.
     * Modifying the currentAppState will mean the undraw function will not be able
     * to undraw it later.
     */
    // UNUSED(keysPressedBefore);
    //UNUSED(keysPressedNow);
    AppState nextAppState = *currentAppState;
    nextAppState.levelChange = 0;
    if (currentAppState->toSave > 0) {
        nextAppState.toSave = currentAppState->toSave - 1;
    }
    if (currentAppState->checkpoint > 0) {
        nextAppState.checkpoint = currentAppState->checkpoint - 1;
    }
    if (KEY_JUST_PRESSED(BUTTON_L, keysPressedNow, keysPressedBefore)) {
        clearAllShots(&nextAppState);
        nextAppState.thePlayerCharacter->xpos = currentAppState->currentSave->xpos;
        nextAppState.thePlayerCharacter->ypos = currentAppState->currentSave->ypos;
        nextAppState.thePlayerCharacter->yvel = currentAppState->currentSave->yvel;
        nextAppState.thePlayerCharacter->airFrames = currentAppState->currentSave->airFrames;
        nextAppState.thePlayerCharacter->direction = currentAppState->currentSave->direction;
        nextAppState.roomNum = currentAppState->currentSave->roomNum;
        nextAppState.room = gameRooms[nextAppState.roomNum];
        nextAppState.thePlayerCharacter->doubleJump = 1;
        nextAppState.levelChange = 1;
        nextAppState.toSave = 0;
        return nextAppState;
    }
    if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore)) {
        clearAllShots(&nextAppState);
        nextAppState.thePlayerCharacter->xpos = currentAppState->checkpointSave->xpos;
        nextAppState.thePlayerCharacter->ypos = currentAppState->checkpointSave->ypos;
        nextAppState.thePlayerCharacter->yvel = 0;
        nextAppState.thePlayerCharacter->airFrames = 0;
        nextAppState.thePlayerCharacter->direction = 1;
        nextAppState.roomNum = currentAppState->checkpointSave->roomNum;
        nextAppState.room = gameRooms[nextAppState.roomNum];
        nextAppState.thePlayerCharacter->doubleJump = 1;
        nextAppState.levelChange = 1;
        nextAppState.toSave = 0;
        return nextAppState;
    }
    if (KEY_DOWN(BUTTON_LEFT, keysPressedNow) ) {
        nextAppState.thePlayerCharacter->xvel = -1;
    } else if (KEY_DOWN(BUTTON_RIGHT, keysPressedNow)) {
        nextAppState.thePlayerCharacter->xvel = 1;
    } else {
        nextAppState.thePlayerCharacter->xvel = 0;
    }
    if (nextAppState.thePlayerCharacter->xvel < 0 && ((checkLeftCollision(currentAppState) & GROUND_KILL))) {
        nextAppState.deathCount = currentAppState->deathCount + 1;
        nextAppState.gameOver = 1;
        return nextAppState;
    } else if ((nextAppState.thePlayerCharacter->xvel > 0)&& ((checkRightCollision(currentAppState) & GROUND_KILL))) {
        nextAppState.deathCount = currentAppState->deathCount + 1;
        nextAppState.gameOver = 1;
        return nextAppState;
    }
    if (nextAppState.thePlayerCharacter->xvel < 0 && ((checkLeftCollision(currentAppState) & GROUND_REGULAR))) {
        nextAppState.thePlayerCharacter->xvel = 0;
    } else if ((nextAppState.thePlayerCharacter->xvel > 0)&& ((checkRightCollision(currentAppState) & GROUND_REGULAR))) {
        nextAppState.thePlayerCharacter->xvel = 0;
    }
    
    if (vBlankCounter % 1 == 0) {
        if (nextAppState.thePlayerCharacter->xvel > 0) {
            if (checkRightCollision(currentAppState) & COLLISION_ADVANCE) {
                nextAppState.roomNum = (currentAppState->roomNum + 1) % NUM_ROOMS;
                nextAppState.room = gameRooms[nextAppState.roomNum];
                nextAppState.thePlayerCharacter->xpos = 2;
                clearAllShots(&nextAppState);
                nextAppState.levelChange = 1;
                if ((nextAppState.roomNum - 5) % 3 == 0) {
                    makeCheckpoint(&nextAppState);
                }
            } else {
                nextAppState.thePlayerCharacter->xpos++;
                nextAppState.thePlayerCharacter->direction = 1;
            }
        } else if ((nextAppState.thePlayerCharacter->xvel < 0)) {
            if (checkLeftCollision(currentAppState) & COLLISION_BACK) {
                nextAppState.roomNum = (currentAppState->roomNum - 1);
                nextAppState.room = gameRooms[nextAppState.roomNum];
                nextAppState.thePlayerCharacter->xpos = WIDTH - 18;
                clearAllShots(&nextAppState);
                nextAppState.levelChange = 1;
            } else {
                nextAppState.thePlayerCharacter->xpos--;
                nextAppState.thePlayerCharacter->direction = 0;
            }
        }
    }
    if ((checkGroundCollision(&nextAppState)) & GROUND_KILL) {
        nextAppState.deathCount = currentAppState->deathCount + 1;
        nextAppState.gameOver = 1;
        return nextAppState;
    }
    if (checkGroundCollision(currentAppState) & GROUND_REGULAR) {
        nextAppState.thePlayerCharacter->yvel = 0;
        nextAppState.thePlayerCharacter->doubleJump = 1;
    } else {
        if (currentAppState->thePlayerCharacter->airFrames >= 7) {
            nextAppState.thePlayerCharacter->yvel--;
            if (nextAppState.thePlayerCharacter->yvel < -2) {
                nextAppState.thePlayerCharacter->yvel = -2;
            }
            nextAppState.thePlayerCharacter->airFrames = 0;
        } else {
            nextAppState.thePlayerCharacter->airFrames++;
        }
    }
    if (KEY_JUST_PRESSED(BUTTON_A, keysPressedNow, keysPressedBefore)) {
        if (checkGroundCollision(currentAppState)) {
            nextAppState.thePlayerCharacter->yvel = 2;
            nextAppState.thePlayerCharacter->airFrames = 0;
        } else if (currentAppState->thePlayerCharacter->doubleJump) {
            nextAppState.thePlayerCharacter->yvel = 2;
            nextAppState.thePlayerCharacter->airFrames = 0;
            nextAppState.thePlayerCharacter->doubleJump = 0;
        }
    }
    if (nextAppState.thePlayerCharacter->yvel > 0) {
        int topCollision = checkTopCollision(currentAppState);
        if (topCollision & GROUND_KILL) {
            nextAppState.deathCount = currentAppState->deathCount + 1;
            nextAppState.gameOver = 1;
            return nextAppState;
        } else if (topCollision & GROUND_REGULAR) {
            nextAppState.thePlayerCharacter->yvel = 0;
            nextAppState.thePlayerCharacter->airFrames = 0;
        }
    }
    
    if (nextAppState.thePlayerCharacter->yvel != 0) {
        int signy = (nextAppState.thePlayerCharacter->yvel > 0) ? 1 : -1;
        if (abs(nextAppState.thePlayerCharacter->yvel) == 2) {
            nextAppState.thePlayerCharacter->ypos -= signy;
        } else {
            if (nextAppState.thePlayerCharacter->airFrames % 2 == 0) {
                nextAppState.thePlayerCharacter->ypos -=signy;
            }
        }
    }
    updateShot(currentAppState, &nextAppState, currentAppState->shot0, nextAppState.shot0);
    updateShot(currentAppState, &nextAppState, currentAppState->shot1, nextAppState.shot1);
    updateShot(currentAppState, &nextAppState, currentAppState->shot2, nextAppState.shot2);
    updateShot(currentAppState, &nextAppState, currentAppState->shot3, nextAppState.shot3);

    if (KEY_JUST_PRESSED(BUTTON_R, keysPressedNow, keysPressedBefore)) {
        Shot* shotToUse = getAvailableShot(currentAppState, &nextAppState);
        if (shotToUse != NULL) {
            shotToUse->inUse = 1;
            if (currentAppState->thePlayerCharacter->direction) {
                shotToUse->xpos = currentAppState->thePlayerCharacter->xpos + 8;
            } else {
                shotToUse->xpos = currentAppState->thePlayerCharacter->xpos - 8;
            }
            shotToUse->ypos = currentAppState->thePlayerCharacter->ypos + 4;
            shotToUse->direction = currentAppState->thePlayerCharacter->direction;
        }
    }
    return nextAppState;
}
AppState processDeadAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow) {
    AppState nextAppState = *currentAppState;
    if (KEY_JUST_PRESSED(BUTTON_L, keysPressedNow, keysPressedBefore)) {
        clearAllShots(&nextAppState);
        nextAppState.thePlayerCharacter->xpos = currentAppState->currentSave->xpos;
        nextAppState.thePlayerCharacter->ypos = currentAppState->currentSave->ypos;
        nextAppState.thePlayerCharacter->yvel = currentAppState->currentSave->yvel;
        nextAppState.thePlayerCharacter->airFrames = currentAppState->currentSave->airFrames;
        nextAppState.thePlayerCharacter->direction = currentAppState->currentSave->direction;
        nextAppState.roomNum = currentAppState->currentSave->roomNum;
        nextAppState.room = gameRooms[nextAppState.roomNum];
        nextAppState.thePlayerCharacter->doubleJump = 1;
        nextAppState.levelChange = 2;
        nextAppState.toSave = 0;
        nextAppState.gameOver = 0;
        return nextAppState;
    }
    if (KEY_JUST_PRESSED(BUTTON_B, keysPressedNow, keysPressedBefore)) {
        clearAllShots(&nextAppState);
        nextAppState.thePlayerCharacter->xpos = currentAppState->checkpointSave->xpos;
        nextAppState.thePlayerCharacter->ypos = currentAppState->checkpointSave->ypos;
        nextAppState.thePlayerCharacter->yvel = 0;
        nextAppState.thePlayerCharacter->airFrames = 0;
        nextAppState.thePlayerCharacter->direction = 1;
        nextAppState.roomNum = currentAppState->checkpointSave->roomNum;
        nextAppState.room = gameRooms[nextAppState.roomNum];
        nextAppState.thePlayerCharacter->doubleJump = 1;
        nextAppState.levelChange = 2;
        nextAppState.toSave = 0;
        nextAppState.gameOver = 0;
        return nextAppState;
    }
    return nextAppState;
}
