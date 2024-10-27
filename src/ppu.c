#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"

// ppu modes
#define MODE_HBLANK 0
#define MODE_VBLANK 1
#define MODE_OAM_SCAN 2
#define MODE_DRAWING 3

// screen dimensions
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define MAX_SPRITES_PER_LINE 10

// memory regions
#define VRAM_START 0x8000
#define VRAM_END 0x97FF
#define OAM_START 0xFE00
#define OAM_END 0xFE9F

// registers
#define LCDC 0xFF40 // lcd control
#define STAT 0xFF41 // lcd status
#define SCY  0xFF42 // scroll y
#define SCX  0xFF43 // scroll x
#define LY   0xFF44 // current scanline
#define LYC  0xFF45 // scanline compare
#define WY   0xFF4A // window y position
#define WX   0xFF4B // window x position minus 7
#define BGP  0xFF47 // bg palette data
#define OBP0 0xFF48 // obj palette 0 data
#define OBP1 0xFF49 // obj palette 1 data

// lcd control bit flags
#define LCDC_ENABLE      (1 << 7)
#define LCDC_WINDOW_MAP  (1 << 6)
#define LCDC_WINDOW_ON   (1 << 5)
#define LCDC_TILE_SEL    (1 << 4)
#define LCDC_BG_MAP      (1 << 3)
#define LCDC_OBJ_SIZE    (1 << 2)
#define LCDC_OBJ_ON      (1 << 1)
#define LCDC_BG_ON       (1 << 0)

// stat bit flags
#define STAT_LYC_INT     (1 << 6)
#define STAT_OAM_INT     (1 << 5)
#define STAT_VBLANK_INT  (1 << 4)
#define STAT_HBLANK_INT  (1 << 3)
#define STAT_LYC_EQUAL   (1 << 2)
#define STAT_MODE_MASK   0x03

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

// implement ppu as a finite-state machine
// ppu step

void ppu_init(ppu *ppu, bus *bus) {
    ppu->bus = bus;
    ppu->vram = &bus->memory[VRAM_START];
    ppu->oam = &bus->memory[OAM_START];
    
    ppu->mode = MODE_OAM_SCAN;
    ppu->current_ly = 0;
    ppu->dot_counter = 0;
    ppu->sprite_count = 0;
    ppu->stat_irq_blocked = 0;
    
    memset(ppu->screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
}

// uint8_t ppu_read_register(ppu *ppu, uint16_t address) {
//     switch(address) {
//         case LCDC:
//             return bus_read8(ppu->bus, LCDC);
            
//         case STAT: {
//             uint8_t stat = bus_read8(ppu->bus, STAT);
//             // bit 7 is unused and always returns 1
//             return (stat | 0x80) & 0xFC | ppu->mode;
//         }
            
//         case LY:
//             return ppu->current_ly;
            
//         case LYC:
//             return bus_read8(ppu->bus, LYC);
            
//         default:
//             return bus_read8(ppu->bus, address);
//     }
// }

void ppu_write_register(ppu *ppu, uint16_t address, uint8_t value) {
    switch(address) {
        case LCDC:
            bus_write8(ppu->bus, LCDC, value);
            // if lcd is being disabled
            if (!(value & LCDC_ENABLE)) {
                ppu->mode = MODE_HBLANK;
                ppu->current_ly = 0;
                ppu->dot_counter = 0;
                // clear screen buffer
                memset(ppu->screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
            }
            break;
            
        case STAT:
            
            bus_write8(ppu->bus, STAT, (value & 0x78) | (bus_read8(ppu->bus, STAT) & 0x07) | 0x80);
            
            break;
            
        case LYC:
            bus_write8(ppu->bus, LYC, value);
            ppu_check_lyc(ppu);
            break;
            
        default:
            bus_write8(ppu->bus, address, value);
            break;
    }
}

void ppu_check_lyc(ppu *ppu) {
    uint8_t stat = bus_read8(ppu->bus, STAT);
    uint8_t lyc = bus_read8(ppu->bus, LYC);
    
    if (ppu->current_ly == lyc) {
        stat |= STAT_LYC_EQUAL;
        if (stat & STAT_LYC_INT && !ppu->stat_irq_blocked) {
            // trigger STAT interrupt
            uint8_t if_reg = bus_read8(ppu->bus, 0xFF0F);
            bus_write8(ppu->bus, 0xFF0F, if_reg | 0x02);
            ppu->stat_irq_blocked = 1;
        }
    } else {
        stat &= ~STAT_LYC_EQUAL;
        ppu->stat_irq_blocked = 0;
    }
    
    bus_write8(ppu->bus, STAT, stat);
}

void ppu_check_stat_interrupt(ppu *ppu) {
    uint8_t stat = bus_read8(ppu->bus, STAT);
    uint8_t request = 0;
    
    // check each stat interrupt condition
    switch(ppu->mode) {
        case MODE_HBLANK:
            request = (stat & STAT_HBLANK_INT);
            break;
        case MODE_VBLANK:
            request = (stat & STAT_VBLANK_INT);
            break;
        case MODE_OAM_SCAN:
            request = (stat & STAT_OAM_INT);
            break;
    }
    
    // if lyc=ly is enabled and matches, that's another condition
    if ((stat & STAT_LYC_INT) && (stat & STAT_LYC_EQUAL)) {
        request = 1;
    }
    
    // if any condition is met and interrupts aren't blocked
    if (request && !ppu->stat_irq_blocked) {
        uint8_t if_reg = bus_read8(ppu->bus, 0xFF0F);
        bus_write8(ppu->bus, 0xFF0F, if_reg | 0x02);
        ppu->stat_irq_blocked = 1;
    } else if (!request) {
        ppu->stat_irq_blocked = 0;
    }
}

void ppu_update_stat(ppu *ppu) {
    uint8_t stat = bus_read8(ppu->bus, STAT);
    // clear mode bits (0-1) and set new mode
    stat = (stat & 0xFC) | ppu->mode;
    // update coincidence flag (bit 2) based on LY=LYC comparison
    if (ppu->current_ly == bus_read8(ppu->bus, LYC)) {
        stat |= (1 << 2);
    } else {
        stat &= ~(1 << 2);
    }
    bus_write8(ppu->bus, STAT, stat);
}

// ppu mode functions

// OAM scan (mode 2):
// - entered at the start of every scanline, except for V-Blank before pixels are drawn to the screen
// - ppu searches for sprites that should be rendered on the current scanline and stores them in a buffer
// total of 80 t-cycles, meaning that the ppu checks a new OAM entry every 2 t-cycles
// - a sprite is only added to the buffer if all of these conditions apply:
// - sprite x position must be > 0
// - LY + 16 must be >= sprite y position
// - LY + 16 must be < sprite y position + the sprite height 
// - the amount of sprites stored in the OAM buffer must be less than 10

void ppu_oam_scan(ppu *ppu) {
    uint8_t lcdc = bus_read8(ppu->bus, LCDC);
    uint8_t sprite_height = (lcdc & LCDC_OBJ_SIZE) ? 16 : 8;
    
    // reset sprite count for new scanline
    ppu->sprite_count = 0;
    
    // perform oam scan if sprites are enabled
    if (lcdc & LCDC_OBJ_ON) {
        // scan all 40 sprites in oam
        for (int i = 0; i < 40 && ppu->sprite_count < MAX_SPRITES_PER_LINE; i++) {
            // each sprite uses 4 bytes in oam
            uint16_t sprite_addr = OAM_START + (i * 4);
            
            // read sprite attributes
            uint8_t y_pos = ppu->oam[i * 4];
            uint8_t x_pos = ppu->oam[i * 4 + 1];
            uint8_t tile_num = ppu->oam[i * 4 + 2];
            uint8_t flags = ppu->oam[i * 4 + 3];
            
            // check if sprite is on current scanline
            int16_t sprite_row = (ppu->current_ly + 16) - y_pos;
            
            if (x_pos > 0 && sprite_row >= 0 && sprite_row < sprite_height) {
                
                // add sprite to buffer
                sprite_data *sprite = &ppu->sprite_buffer[ppu->sprite_count];
                sprite->y_pos = y_pos;
                sprite->x_pos = x_pos;
                sprite->tile_num = tile_num;
                sprite->flags = flags;
                sprite->index = i;
                
                ppu->sprite_count++;
            }
        }
    }
}

// drawing (mode 3):
// - this mode is where the ppu 'draws' pixels on the screen
// - duration depends on multiple variables

void ppu_draw(ppu *ppu) {

}

// h-blank (mode 0):
// - this mode takes up the remainder of the scanline after the drawing mode 3 wraps up
// - essentially pads the duration of the scanline to 456 t-cycles, pausing the ppu 

// void ppu_hblank(ppu *ppu) {
//     ppu->dot_counter++;
// }

// v-blank (mode 1):
// - same as h-blank except instead of taking place at the end of every scanline, it's a much
// longer period at the end of every frame
// - takes 456 t-cycles
// - 154 scanlines take place for each frame, so v-blank happens once every 154 scanline reps

void ppu_vblank(ppu *ppu) {
    
}

void ppu_step(ppu *ppu) {
    
    // check if lcd is enabled
    if (!(bus_read8(ppu->bus, LCDC) & LCDC_ENABLE)) {
        return;
    }

    ppu->dot_counter++;

    switch(ppu->mode) {
        case MODE_OAM_SCAN:
            // entered at the start of every scanline
            if (ppu->dot_counter == 1) { 
                ppu_oam_scan(ppu);
            }
            // total of 80 t-cycles 
            if (ppu->dot_counter >= 80) { 
                ppu->mode = MODE_DRAWING;
                ppu->dot_counter = 0;
                ppu_update_stat(ppu);
                ppu_check_stat_interrupt(ppu);
            }

            break;

        case MODE_DRAWING:
            break;

        case MODE_HBLANK:
            while (ppu->dot_counter < 456) {
                ppu->dot_counter++;
            }
            break;

        case MODE_VBLANK:
            break;
            
    
}