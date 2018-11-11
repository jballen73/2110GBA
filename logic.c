#include "logic.h"
#include "graphics.h"
#include "images/basicScreen.h"
#include "images/jumpScreen.h"
#include <stdlib.h>
//extern volatile OamEntry* shadow;
static Room **gameRooms;
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
    room2->backgroundImage = basicScreen;
    room2->collisionMap = basicScreenCollision;
    gameRooms[2] = room2;

    Character *newPlayerCharacter =  (Character*)malloc(sizeof(Character));
    newPlayerCharacter->xvel = 0;
    newPlayerCharacter->yvel = 0;
    newPlayerCharacter->xpos = 30;
    newPlayerCharacter->ypos = 100;
    newPlayerCharacter->doubleJump = 1;
    newPlayerCharacter->airFrames = 0;
    newPlayerCharacter->direction = 1;
    Shot *newShot0 = (Shot*)malloc(sizeof(Shot));
    newShot0->xpos = WIDTH;
    newShot0->ypos = HEIGHT;
    newShot0->inUse = 0;
    appState->thePlayerCharacter = newPlayerCharacter;
    appState->shot0 = newShot0;
    appState->roomNum = 0;
    appState->room = gameRooms[appState->roomNum];
    appState->gameOver = 0;
    appState->levelChange = 0;
}

// TA-TODO: Add any process functions for sub-elements of your app here.
// For example, for a snake game, you could have a processSnake function
// or a createRandomFood function or a processFoods function.
//
// e.g.:
// static Snake processSnake(Snake* currentSnake);
// static void generateRandomFoods(AppState* currentAppState, AppState* nextAppState);

static u16 getBackgroundPixel(AppState *state, int xpos, int ypos) {
    return state->room->collisionMap[OFFSET(ypos, xpos, WIDTH)];
}

static u16 checkGroundCollision(AppState *state) {
    u16 result = 0;
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + i,
        state->thePlayerCharacter->ypos + 16);
        result = result | (pixel == 0x0000);
        result = result | ((pixel == GROUND_KILL_VALUE) <<4);
        
    }
    // if (result & GROUND_KILL) {
    //     state->room = gameRooms[1];
    // }
    return result;
}
static int checkLeftCollision(AppState *state) {
    int result = 0;
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos - 1,
        state->thePlayerCharacter->ypos + i);
        result = result | (pixel == 0x0000) | ((pixel == GROUND_KILL_VALUE) << 1) | ((pixel == WALL_BACK) << 2);
        
    }
    return result;
}
static int checkRightCollision(AppState *state) {
    int result = 0;
    for (int i = 0; i < 16; i++) {
        u16 pixel = getBackgroundPixel(state, state->thePlayerCharacter->xpos + 16,
        state->thePlayerCharacter->ypos + i);
        result = result | (pixel == 0x0000) | ((pixel == GROUND_KILL_VALUE) << 1) | ((pixel == WALL_ADVANCE) << 3);
        
    }
    return result;
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
    if (KEY_DOWN(BUTTON_LEFT, keysPressedNow) ) {
        nextAppState.thePlayerCharacter->xvel = -1;
    } else if (KEY_DOWN(BUTTON_RIGHT, keysPressedNow)) {
        nextAppState.thePlayerCharacter->xvel = 1;
    } else {
        nextAppState.thePlayerCharacter->xvel = 0;
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
                nextAppState.levelChange = 5;
            } else {
                nextAppState.thePlayerCharacter->xpos++;
                nextAppState.thePlayerCharacter->direction = 1;
            }
        } else if ((nextAppState.thePlayerCharacter->xvel < 0)) {
            if (checkLeftCollision(currentAppState) & COLLISION_BACK) {
                nextAppState.roomNum = (currentAppState->roomNum - 1);
                nextAppState.room = gameRooms[nextAppState.roomNum];
                nextAppState.thePlayerCharacter->xpos = WIDTH - 18;
                nextAppState.levelChange = 5;
            } else {
                nextAppState.thePlayerCharacter->xpos--;
                nextAppState.thePlayerCharacter->direction = 0;
            }
        }
    }
    if ((checkGroundCollision(&nextAppState)) & GROUND_KILL) {
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
    if (currentAppState->shot0->inUse) {
        if (currentAppState->shot0->xpos >= WIDTH || currentAppState->shot0->xpos < 0) {
            nextAppState.shot0->inUse = 0;
            nextAppState.shot0->xpos = WIDTH;
        } else {
            if (currentAppState->shot0->direction) {
                nextAppState.shot0->xpos = currentAppState->shot0->xpos + 2;
            } else {
                nextAppState.shot0->xpos = currentAppState->shot0->xpos - 2;
            }
        }
    }
    if (KEY_JUST_PRESSED(BUTTON_R, keysPressedNow, keysPressedBefore)) {
        nextAppState.shot0->inUse = 1;
        if (currentAppState->thePlayerCharacter->direction) {
            nextAppState.shot0->xpos = currentAppState->thePlayerCharacter->xpos + 8;
        } else {
            nextAppState.shot0->xpos = currentAppState->thePlayerCharacter->xpos - 8;
        }
        nextAppState.shot0->ypos = currentAppState->thePlayerCharacter->ypos + 4;
        nextAppState.shot0->direction = currentAppState->thePlayerCharacter->direction;

    }
    return nextAppState;
}
