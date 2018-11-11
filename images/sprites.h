/*
 * Exported with nin10kit v1.7
 * Invocation command was nin10kit --mode=sprites --bpp=4 --for_bitmap sprites playerShot.png playerCharacterSprite.png --transparent=FFFFFF 
 * Time-stamp: Saturday 11/10/2018, 20:50:39
 * 
 * Image Information
 * -----------------
 * playerShot.png 8@8
 * playerCharacterSprite.png 16@16
 * Transparent color: (255, 255, 255)
 * 
 * All bug reports / feature requests are to be filed here https://github.com/TricksterGuy/nin10kit/issues
 */

#ifndef SPRITES_H
#define SPRITES_H

#define SPRITES_PALETTE_TYPE (0 << 13)
#define SPRITES_DIMENSION_TYPE (1 << 6)

extern const unsigned short sprites_palette[256];
#define SPRITES_PALETTE_SIZE 512
#define SPRITES_PALETTE_LENGTH 256

extern const unsigned short sprites[80];
#define SPRITES_SIZE 160
#define SPRITES_LENGTH 80

#define PLAYERCHARACTERSPRITE_PALETTE_ID (0 << 12)
#define PLAYERCHARACTERSPRITE_SPRITE_SHAPE (0 << 14)
#define PLAYERCHARACTERSPRITE_SPRITE_SIZE (1 << 14)
#define PLAYERCHARACTERSPRITE_ID 512

#define PLAYERSHOT_PALETTE_ID (0 << 12)
#define PLAYERSHOT_SPRITE_SHAPE (0 << 14)
#define PLAYERSHOT_SPRITE_SIZE (0 << 14)
#define PLAYERSHOT_ID 516

#endif

