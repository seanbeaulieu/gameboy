#ifndef PPU_H
#define PPU_H

#include <bus.h>
#include <cpu.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ppu modes
#define MODE_HBLANK 0
#define MODE_VBLANK 1
#define MODE_OAM_SCAN 2
#define MODE_DRAWING 3

// screen dimensions
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define TILES_PER_LINE 20
#define TILES_PER_COLUMN 18

typedef struct ppu {

} ppu;

#endif