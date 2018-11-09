/*
 * Exported with nin10kit v1.7
 * Invocation command was nin10kit --mode=sprites --bpp=4 --for_bitmap playerChar playerCharacterSprite.png --transparent=FFFFFF 
 * Time-stamp: Wednesday 11/07/2018, 22:18:38
 * 
 * Image Information
 * -----------------
 * playerCharacterSprite.png 16@16
 * Transparent color: (255, 255, 255)
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef PLAYERCHAR_H
#define PLAYERCHAR_H

#define PLAYERCHAR_PALETTE_TYPE (0 << 13)
#define PLAYERCHAR_DIMENSION_TYPE (1 << 6)

extern const unsigned short playerChar_palette[256];
#define PLAYERCHAR_PALETTE_SIZE 512
#define PLAYERCHAR_PALETTE_LENGTH 256

extern const unsigned short playerChar[64];
#define PLAYERCHAR_SIZE 128
#define PLAYERCHAR_LENGTH 64

#define PLAYERCHARACTERSPRITE_PALETTE_ID (0 << 12)
#define PLAYERCHARACTERSPRITE_SPRITE_SHAPE (0 << 14)
#define PLAYERCHARACTERSPRITE_SPRITE_SIZE (1 << 14)
#define PLAYERCHARACTERSPRITE_ID 512

#endif

