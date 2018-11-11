#ifndef LOGIC_SEEN
#define LOGIC_SEEN

#include "gba.h"

#define GROUND_NONE (0)
#define GROUND_REGULAR (1)
#define GROUND_KILL (1<<4)
#define COLLISION_BACK (1<<2)
#define COLLISION_ADVANCE (1<<3)

#define GROUND_KILL_VALUE (0x107d)
#define WALL_BACK (0x7680)
#define WALL_ADVANCE (0x26c4)

#define NUM_ROOMS (3)

typedef struct {
    int xvel;
    int yvel;
    int xpos;
    int ypos;
    int doubleJump;
    int airFrames;
    int direction;
} Character;
typedef struct  {
    int xpos;
    int ypos;
    int inUse;
    int direction;
} Shot;
typedef struct {
    const u16 *backgroundImage;
    const u16 *collisionMap;
} Room;

typedef struct {
    // Store whether or not the game is over in this member:
    int gameOver;

    /*
    * TA-TODO: Add any logical elements you need to keep track of in your app.
    *
    * For example, for a Snake game, those could be:
    *
    * Snake snake;
    * Food foods[10];
    * int points;
    *
    */
   Character* thePlayerCharacter;
   Shot* shot0;
   Shot* shot1;
   Shot* shot2;
   Shot* shot3;
   Room* room;
   u16 roomNum;
   u8 levelChange;
} AppState;

/*
* TA-TODO: Add any additional structs that you need for your app.
*
* For example, for a Snake game, one could be:
*
* typedef struct {
*   int heading;
*   int length;
*   int x;
*   int y;
* } Snake;
*
*/
// This function can initialize an unused AppState struct.
void initializeAppState(AppState *appState);

// This function will be used to process app frames.
AppState processAppState(AppState *currentAppState, u32 keysPressedBefore, u32 keysPressedNow);

// If you have anything else you need accessible from outside the logic.c
// file, you can add them here. You likely won't.
#endif
