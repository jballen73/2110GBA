#include "gba.h"
#include "logic.h"
#include "graphics.h"
// TA-TODO: Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
#include "images/garbage.h"
#include "images/basicScreen.h"
#include "images/playerChar.h"

#include <stdio.h>
#include <stdlib.h>
// AppState enum definition
typedef enum {
    // TA-TODO: Add any additional states you need for your app.
    START,
    START_NODRAW,
    APP_INIT,
    APP,
    APP_EXIT,
    APP_EXIT_NODRAW,
} GBAState;

int main(void) {
    // TA-TODO: Manipulate REG_DISPCNT here to set Mode 3.
    REG_DISPCNT = MODE3 | BG2_ENABLE | OBJ_ENABLE | PLAYERCHAR_DIMENSION_TYPE;

    GBAState state = START;

    // We store the "previous" and "current" states.
    AppState currentAppState, nextAppState;

    // We store the current and previous values of the button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;
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
            
            fillScreenDMA(BLUE);
            state = START_NODRAW;
            break;
        case START_NODRAW:
            // TA-TODO: Check for a button press here to start the app.
            // Start the app by switching the state to APP_INIT.
            if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) {
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
                state = START;
                break;
            }
            nextAppState = processAppState(&currentAppState, previousButtons, currentButtons);

            // Wait for vblank before we do any drawing.
            waitForVBlank();

            // Undraw the previous state
            undrawAppState(&currentAppState);

            // Draw the current state
            drawAppState(&nextAppState);

            // Now set the current state as the next state for the next iter.
            currentAppState = nextAppState;

            // Check if the app is exiting. If it is, then go to the exit state.
            if (nextAppState.gameOver) state = APP_EXIT;

            break;
        case APP_EXIT:
            // Wait for vblank
            waitForVBlank();

            // TA-TODO: Draw the exit / gameover screen

            state = APP_EXIT_NODRAW;
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
