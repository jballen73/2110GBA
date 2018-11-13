#include "gba.h"
#include "logic.h"
#include "graphics.h"
// TA-TODO: Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
#include "images/garbage.h"
#include "images/basicScreen.h"
#include "images/deathText.h"
#include "images/titleScreen.h"
#include "images/sprites.h"

#include <stdio.h>
#include <stdlib.h>
// AppState enum definition
typedef enum {
    // TA-TODO: Add any additional states you need for your app.
    START,
    START_WAIT_FOR_INPUT,
    START_AFTER_BUTTON,
    APP_INIT,
    APP,
    APP_DEAD,
    APP_EXIT_NODRAW,
} GBAState;

int main(void) {
    // TA-TODO: Manipulate REG_DISPCNT here to set Mode 3.
    REG_DISPCNT = MODE3 | BG2_ENABLE | OBJ_ENABLE | SPRITES_DIMENSION_TYPE;

    GBAState state = START;

    // We store the "previous" and "current" states.
    AppState currentAppState, nextAppState;

    // We store the current and previous values of the button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;
    int startFrameCounter = 0;
    while(1) {
        // Load the current state of the buttons
        currentButtons = BUTTONS;

        // TA-TODO: Manipulate the state machine below as needed.
        switch(state) {
        case START:
            // Wait for vblank
            waitForVBlank();
            graphicsInit();
            // TA-TODO: Draw the start state here.
            
            state = START_WAIT_FOR_INPUT;
            break;
        case START_WAIT_FOR_INPUT:
            waitForVBlank();
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
                state = START_AFTER_BUTTON;
                startFrameCounter = 0;
            }
            if (vBlankCounter %  30 < 15) {
                drawFullScreenImageDMA(titleScreenStartText);
            } else {
                drawFullScreenImageDMA(titleScreenNoStartText);
            }
            break;
        case START_AFTER_BUTTON:
            waitForVBlank();
            if (startFrameCounter < 50) {
                if (startFrameCounter % 8 < 4) {
                    drawFullScreenImageDMA(titleScreenStartText);
                } else {
                    drawFullScreenImageDMA(titleScreenNoStartText);
                }
            } else {
                drawFullScreenImageDMA(titleScreenStartText);
            }
            startFrameCounter++;
            if (startFrameCounter >= 150) {
                state = APP_INIT;
            }
            break;
        case APP_INIT:
            // Initialize the app. Switch to the APP state.
            //graphicsInit();
            initializeAppState(&currentAppState);

            // Draw the initial state of the app
            fullDrawAppState(&currentAppState);
             state = APP;
            break;
        case APP:
            // Process the app for one frame, store the next state
            if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
                hideSprites();
                state = START;
                break;
            }
            nextAppState = processAppState(&currentAppState, previousButtons, currentButtons);

            // Wait for vblank before we do any drawing.
            waitForVBlank();

            // Undraw the previous state
            undrawAppState(&currentAppState);

            // Draw the current state
            if (nextAppState.levelChange) {
                fullDrawAppState(&nextAppState);
            }
            drawAppState(&nextAppState);

            // Now set the current state as the next state for the next iter.
            currentAppState = nextAppState;

            // Check if the app is exiting. If it is, then go to the exit state.
            if (nextAppState.gameOver) {
                waitForVBlank();
                fullDrawAppState(&nextAppState);
                drawAppState(&nextAppState);
                state = APP_DEAD;
            }

            break;
        case APP_DEAD:
            // Wait for vblank
            waitForVBlank();

            // TA-TODO: Draw the exit / gameover screen
            hideSprites();
            drawImageDMA(69, 61, DEATHTEXT_WIDTH, DEATHTEXT_HEIGHT, deathText);
            nextAppState = processDeadAppState(&currentAppState, previousButtons, currentButtons);
            if (!nextAppState.gameOver) {
                fullDrawAppState(&nextAppState);
                state = APP;
            }
            currentAppState = nextAppState;
            //state = APP_EXIT_NODRAW;
            break;
        case APP_EXIT_NODRAW:
            // TA-TODO: Check for a button press here to go back to the start screen

            break;
        }

        // Store the current state of the buttons
        previousButtons = currentButtons;
    }

    return 0;
}
