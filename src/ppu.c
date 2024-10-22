#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../include/cpu.h"
#include "../include/bus.h"

// the screen is a 160x144 pixel LCD made up of 8x8 tiles

// these tiles are 16 bytes per row and contain 8 lines of 8 pixels
// 2 bits are used per pixel to store the color pallete information
// so for 2 bytes per row, the pixel color is determined by the bit 
// at the corresponding first byte, as well as the corresponding spot
// in the second byte

// the background is a 32x32 tile grid (256x256) which is used for scrolling
// however, the screen is only 20x18 tiles. the scrolling is handled by using
// the SCX and SCY registers. those registers determine the offset pixel amount the screen 
// is away from the left and top borders of the background
// if the screen goes over the border of the background, it wraps back around to the left or top

// the window is another 32x32 tile grid. it goes over the background, and the position is determined by
// the WX and WY registers

// WY register 0xFF4A
// using this register, set the top border of the window at that position held by the WY register

// WX register 0xFF4B
// quirk: calculate the horizontal position the window should be displayed as WX - 7

// sprites are 8x8 or 8x16 pixel tiles which are not related to the background or window. 
// the gameboy can store up to 40 in the 0AM section at (object tiles are stored at) 0x8000-0x8FFF, and have unsigned numbering

// graphics data is stored in VRAM at 0x8000 - 0x97FF
// access these using 'Tile Numbers', which are an index of a 16 byte block in the graphics data
// two different addressing methods:
// 8000 method: uses 0x8000 as a base pointer and adds (TILE_NUMBER * 16) to it - where TILE_NUMBER
// is an unsigned 8-bit integer.
// 8800 method: uses 0x9000 as a base pointer and adds (SIGNED_TILE_NUMBER * 16) to it, where SIGNED_TILE_NUMBER
// is a signed 8-bit integer. starts from 0x9000.
// however, 0xFF would refer to 0x8FF0, 0xFE to 0x8FE0, etc
// these will be used based on bit 4 in the LCDC register

// background maps: 0x9800-0x9BFF and 0x9C00-0x9FFF each contain a background map that determines which tiles should
// be displayed in the background/window grid
// map consists of 32x32 bytes representing tile number organized row by row
// the first byte in a background map is the Tile Number of the Tile at the very top left. The byte after in the map
// is the tile number directly to the right. the 33rd byte represents the tile number of the left tile in the second row

// OAM object attributes memory: 0x8000-0x8FFF
// contains data used to display sprites/objects
// each sprite uses 4 bytes here
// byte 0: y-position: used to determine the vertical position of the sprite on the screen
// actual y position is determined by this byte - 16

// byte 1: x-position: horizontal position of the sprite on the screen
// actual x position is determined by this byte - 8

// byte 2: tile number: represents the tile number used for fetching the graphics data of the sprite
// always uses 8000 addressing method, always an unsigned 8bit integer

// byte 3: sprite flags: contians bit-flags that can apply certain effects and options to a sprite:
// bit 7: 0BJ-to-BG priority:
// - 0 = sprite is always rendered above background
// - 1 = background colors 1-3 overlay sprite, sprite is stillm rendered above color 0 (transparent)
// bit 6:
// - y-flip: if set to 1 the sprite is flipped vertically
// bit 5:
// - x-flip: if set to 1 the sprite is flipped horizontally 
// bit 4:
// - palette number: if set to 0, the OBPO register is used as the palette, otherwise, it is the OBP1 register used as the pallet
// bit 3-0:
// - irrelevant for regular gameboy

// scanline renderer: 
// the PPU operates by setting pixels line by line, left to right, from top to bottom 
// operates on a pixel by pixel basis, and NOT tile by tile
// the number of the scanline the ppu is currently processing is stored in the LY register (interchangable terms)

// PPU modes:
// during a scanline the ppu enters one of four different modes:
//
// OAM scan (mode 2):
// - entered at the start of every scanline, except for V-Blank before pixels are drawn to the screen
// - ppu searches for sprites that should be rendered on the current scanline and stores them in a buffer
// total of 80 t-cycles, meaning that the ppu checks a new OAM entry every 2 t-cycles
// - a sprite is only added to the buffer if all of these conditions apply:
// - sprite x position must be > 0
// - LY + 16 must be >= sprite y position
// - LY + 16 must be < sprite y position + the sprite height 
// - the amount of sprites stored in the OAM buffer must be less than 10
//
// drawing (mode 3):
// - this mode is where the ppu 'draws' pixels on the screen
// - duration depends on multiple variables
//
// h-blank (mode 0):
// - this mode takes up the remainder of the scanline after the drawing mode 3 wraps up
// - essentially pads the duration of the scanline to 456 t-cycles, pausing the ppu 
//
// v-blank (mode 1):
// - same as h-blank except instead of taking place at the end of every scanline, it's a much
// longer period at the end of every frame
// - takes 456 t-cycles
// - 154 scanlines take place for each frame, so v-blank happens once every 154 scanline reps