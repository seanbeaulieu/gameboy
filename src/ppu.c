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

    // window line counter
    ppu->window_visible = false;
    ppu->window_line_counter = 0;
    
    
    memset(ppu->screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);

    // printf("PPU init end - callback ptr: %p\n", (void*)ppu->frame_complete_callback);
}

// set callback
void ppu_set_frame_callback(ppu *ppu, void (*callback)(uint8_t *buffer)) {
    printf("Setting callback - old ptr: %p, new ptr: %p\n", 
           (void*)ppu->frame_complete_callback, (void*)callback);
    ppu->frame_complete_callback = callback;
    printf("Callback set - current ptr: %p\n", (void*)ppu->frame_complete_callback);
}

void ppu_check_stat_interrupts(ppu *ppu) {
    uint8_t stat = bus_read8(ppu->bus, STAT);
    uint8_t if_reg = bus_read8(ppu->bus, 0xFF0F);
    bool interrupt_requested = false;
    
    // LY=LYC check
    uint8_t lyc = bus_read8(ppu->bus, LYC);
    // printf("checking stats - LY:%d LYC:%d STAT before:%02X\n", 
    //        ppu->current_ly, lyc, stat);

    if (ppu->current_ly == lyc) {
        stat |= STAT_LYC_EQUAL;
        // printf("LY=LYC match. STAT after setting equal bit:%02X\n", stat);
        if (stat & STAT_LYC_INT) {
            // printf("LYC interrupt enabled in STAT\n");
            interrupt_requested = true;
        }
    } else {
        stat &= ~STAT_LYC_EQUAL;
    }

    // Mode interrupts
    switch(ppu->mode) {
        case MODE_HBLANK:
            if (stat & STAT_HBLANK_INT) interrupt_requested = true;
            break;
        case MODE_VBLANK:
            if (stat & STAT_VBLANK_INT) interrupt_requested = true;
            break;
        case MODE_OAM_SCAN:
            if (stat & STAT_OAM_INT) interrupt_requested = true;
            break;
    }

    // write back STAT updates
    bus_write8(ppu->bus, STAT, stat);
    // printf("wrote STAT:%02X back to memory\n", stat);

    // request STAT interrupt if needed
    if (interrupt_requested) {
        // printf("stat interrupt requested. Current LY:%d\n", ppu->current_ly);
        // printf("IF before:%02X after:%02X\n", 
        //        if_reg, if_reg | 0x02);
        bus_write8(ppu->bus, 0xFF0F, if_reg | 0x02);
    }
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
            
            if (sprite_row >= 0 && sprite_row < sprite_height && x_pos != 0) {
                
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
// - actually writes to screen buffer 

void ppu_render_scanline(ppu *ppu) {
    uint8_t lcdc = bus_read8(ppu->bus, LCDC);
    uint8_t *scanline = &ppu->screen_buffer[ppu->current_ly * SCREEN_WIDTH];
    
    // if background is enabled
    if (lcdc & !LCDC_BG_ON) {
        printf("LCDC BG OFF");
    }
    if (lcdc & LCDC_BG_ON) {
        uint8_t scy = bus_read8(ppu->bus, SCY);
        uint8_t scx = bus_read8(ppu->bus, SCX);
        uint16_t bg_map = (lcdc & LCDC_BG_MAP) ? 0x9C00 : 0x9800;
        
        // calculate y position in background map
        uint8_t y = (ppu->current_ly + scy) & 0xFF;
        uint8_t tile_y = y >> 3;  // divide by 8
        uint8_t fine_y = y & 7;   // y % 8
        
        // render each pixel in the scanline
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t mapped_x = (x + scx) & 0xFF;
            uint8_t tile_x = mapped_x >> 3;
            uint8_t fine_x = mapped_x & 7;
            
            // get tile number from background map
            uint16_t tile_addr = bg_map + (tile_y * 32) + tile_x;
            uint8_t tile_num = ppu->vram[tile_addr - VRAM_START];
            
            // get tile data address
            uint16_t tile_data;
            if (lcdc & LCDC_TILE_SEL) {
                tile_data = 0x8000 + (tile_num * 16);
            } else {
                tile_data = 0x9000 + ((int8_t)tile_num * 16);
            }

            // get the two bytes for this line of the tile
            uint8_t byte1 = ppu->vram[(tile_data + (fine_y * 2)) - VRAM_START];
            uint8_t byte2 = ppu->vram[(tile_data + (fine_y * 2) + 1) - VRAM_START];
            
            // combine bits for color
            uint8_t bit = 7 - fine_x;
            uint8_t color = ((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1);
            
            // apply background palette
            uint8_t bgp = bus_read8(ppu->bus, BGP);
            color = (bgp >> (color * 2)) & 3;
            
            scanline[x] = color;
        }
    } else {
        // print white
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            scanline[x] = 0;
        }
    }

    // The window becomes visible (if enabled) when positions are set in range WX=0..166, WY=0..143. 
    // A postion of WX=7, WY=0 locates the window at upper left, it is then completly covering normal background.
    if ((lcdc & LCDC_WINDOW_ON) && (lcdc & LCDC_ENABLE)) {
        uint8_t wy = bus_read8(ppu->bus, WY);
        uint8_t wx = bus_read8(ppu->bus, WX);
        
        // check if window coordinates are in valid range
        bool in_range = (wx <= 166 && wy <= 143 && ppu->current_ly >= wy);
        
        if (in_range) {
            // calculate effective window x position
            uint8_t window_x = wx - 7;
            
            // calculate window y using line counter
            uint8_t window_y = ppu->window_line_counter;
            uint8_t tile_y = window_y >> 3;
            uint8_t fine_y = window_y & 7;
            
            // get window tile map
            uint16_t window_map = (lcdc & LCDC_WINDOW_MAP) ? 0x9C00 : 0x9800;
            
            // render window pixels
            for (int x = 0; x < SCREEN_WIDTH - window_x; x++) {
                int screen_x = window_x + x;
                
                if (screen_x >= 0 && screen_x < SCREEN_WIDTH) {
                    uint8_t tile_x = x >> 3;
                    uint8_t fine_x = x & 7;
                    
                    uint16_t tile_addr = window_map + (tile_y * 32) + tile_x;
                    uint8_t tile_num = ppu->vram[tile_addr - VRAM_START];
                    
                    uint16_t tile_data;
                    if (lcdc & LCDC_TILE_SEL) {
                        tile_data = 0x8000 + (tile_num * 16);
                    } else {
                        tile_data = 0x9000 + ((int8_t)tile_num * 16);
                    }
                    
                    uint8_t byte1 = ppu->vram[(tile_data + (fine_y * 2)) - VRAM_START];
                    uint8_t byte2 = ppu->vram[(tile_data + (fine_y * 2) + 1) - VRAM_START];
                    
                    uint8_t bit = 7 - fine_x;
                    uint8_t color = ((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1);
                    
                    uint8_t bgp = bus_read8(ppu->bus, BGP);
                    color = (bgp >> (color * 2)) & 3;
                    
                    ppu->screen_buffer[ppu->current_ly * SCREEN_WIDTH + screen_x] = color;
                }
            }
            
            // increment window counter when window was actually visible and drawn
            ppu->window_line_counter++;
        }
    }
    
    // render sprites if enabled
    if (lcdc & LCDC_OBJ_ON) {
        // sort sprites by x coordinate (ascending) and OAM index for proper priority
        for (int i = 0; i < ppu->sprite_count - 1; i++) {
            for (int j = i + 1; j < ppu->sprite_count; j++) {
                // compare x positions first
                if (ppu->sprite_buffer[j].x_pos < ppu->sprite_buffer[i].x_pos ||
                    // if x positions are equal, earlier OAM index has priority
                    (ppu->sprite_buffer[j].x_pos == ppu->sprite_buffer[i].x_pos && 
                    ppu->sprite_buffer[j].index < ppu->sprite_buffer[i].index)) {
                    // swap sprites
                    sprite_data temp = ppu->sprite_buffer[i];
                    ppu->sprite_buffer[i] = ppu->sprite_buffer[j];
                    ppu->sprite_buffer[j] = temp;
                }
            }
        }

        // render sprites from highest to lowest priority (reverse order)
        // lower priority sprites are drawn first and can be overwritten
        for (int i = ppu->sprite_count - 1; i >= 0; i--) {
            sprite_data *sprite = &ppu->sprite_buffer[i];
            
            // calculate vertical line being drawn on sprite
            int16_t line = ppu->current_ly - (sprite->y_pos - 16);
            
            // if sprite is vertically flipped
            if (sprite->flags & 0x40) {
                line = ((lcdc & LCDC_OBJ_SIZE) ? 15 : 7) - line;
            }
            
            // get tile data address
            // uint16_t tile_addr = 0x8000 + (sprite->tile_num * 16) + (line * 2);

            uint8_t adjusted_tile_num = sprite->tile_num;
            if (lcdc & LCDC_OBJ_SIZE) {
                // mask off bit 0 for 8x16 sprites
                adjusted_tile_num &= 0xFE;  // clear lowest bit
            }
            uint16_t tile_addr = 0x8000 + (adjusted_tile_num * 16) + (line * 2);
            
            // get tile data
            uint8_t byte1 = ppu->vram[tile_addr - VRAM_START];
            uint8_t byte2 = ppu->vram[(tile_addr + 1) - VRAM_START];
            
            // draw all pixels for this line of the sprite
            for (int x = 0; x < 8; x++) {
                int pixel_x = sprite->x_pos - 8 + x;
                
                // check if pixel is on screen
                if (pixel_x >= 0 && pixel_x < SCREEN_WIDTH) {
                    uint8_t bit = sprite->flags & 0x20 ? x : 7 - x;
                    uint8_t color = ((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1);
                    
                    // only draw non-transparent (opaque) pixels
                    if (color > 0) {
                        // apply sprite palette
                        uint8_t palette = bus_read8(ppu->bus, sprite->flags & 0x10 ? OBP1 : OBP0);
                        color = (palette >> (color * 2)) & 3;
                        
                        // check sprite to background priority
                        if (!(sprite->flags & 0x80) || scanline[pixel_x] == 0) {
                            scanline[pixel_x] = color;
                        }
                    }
                }
            }
        }
    }
}

// h-blank (mode 0):
// - this mode takes up the remainder of the scanline after the drawing mode 3 wraps up
// - essentially pads the duration of the scanline to 456 t-cycles, pausing the ppu 

// v-blank (mode 1):
// - same as h-blank except instead of taking place at the end of every scanline, it's a much
// longer period at the end of every frame
// - takes 456 t-cycles
// - 154 scanlines take place for each frame, so v-blank happens once every 154 scanline reps


void ppu_step(ppu *ppu) {
    if (!(bus_read8(ppu->bus, LCDC) & LCDC_ENABLE)) {
        ppu->window_line_counter = 0;
        return;
    }
    
    ppu->dot_counter++;
    // ppu_check_stat_interrupts(ppu);

    switch(ppu->mode) {
        case MODE_OAM_SCAN:
            if (ppu->dot_counter >= 80) {
                // do at end of oam scan as some flags are set during OAM mode
                ppu_oam_scan(ppu);
                ppu->mode = MODE_DRAWING;
                ppu->dot_counter = 0;
                // Update STAT mode bits first
                uint8_t stat = bus_read8(ppu->bus, STAT);
                bus_write8(ppu->bus, STAT, (stat & 0xFC) | ppu->mode);
                ppu_check_stat_interrupts(ppu);
            }
            //ppu_check_stat_interrupts(ppu);
            break;

        case MODE_DRAWING:
            if (ppu->dot_counter >= 172) {
                ppu_render_scanline(ppu);
                ppu->mode = MODE_HBLANK;
                ppu->dot_counter = 0;
                // Update STAT mode bits first
                uint8_t stat = bus_read8(ppu->bus, STAT);
                bus_write8(ppu->bus, STAT, (stat & 0xFC) | ppu->mode);
                ppu_check_stat_interrupts(ppu);
            }
            //ppu_check_stat_interrupts(ppu);
            break;

        case MODE_HBLANK:
            if (ppu->dot_counter >= 456 - (80 + 172)) {
                ppu->current_ly++;
                ppu->dot_counter = 0;
                
                if (ppu->current_ly == 144) {
                    ppu->mode = MODE_VBLANK;
                    // Set both the mode bits and request VBLANK interrupt
                    uint8_t stat = bus_read8(ppu->bus, STAT);
                    bus_write8(ppu->bus, STAT, (stat & 0xFC) | ppu->mode);
                    uint8_t if_reg = bus_read8(ppu->bus, 0xFF0F);
                    bus_write8(ppu->bus, 0xFF0F, if_reg | 0x01);
                    ppu_check_stat_interrupts(ppu);
                } else {
                    ppu->mode = MODE_OAM_SCAN;
                    // Update STAT mode bits first
                    uint8_t stat = bus_read8(ppu->bus, STAT);
                    bus_write8(ppu->bus, STAT, (stat & 0xFC) | ppu->mode);
                    ppu_check_stat_interrupts(ppu);
                }
            }
            //ppu_check_stat_interrupts(ppu);
            break;

        case MODE_VBLANK:
            if (ppu->dot_counter >= 456) {
                ppu->dot_counter = 0;
                ppu->current_ly++;
                //ppu_check_stat_interrupts(ppu);

                if (ppu->current_ly >= 154) {

                    ppu->window_line_counter = 0;

                    if (ppu->frame_complete_callback) {
                        ppu->frame_complete_callback(ppu->screen_buffer);
                    }
                    
                    ppu->current_ly = 0;
                    ppu->mode = MODE_OAM_SCAN;
                    memset(ppu->screen_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
                    // Update STAT mode bits first
                    uint8_t stat = bus_read8(ppu->bus, STAT);
                    bus_write8(ppu->bus, STAT, (stat & 0xFC) | ppu->mode);
                    ppu_check_stat_interrupts(ppu);
                }
                // ppu_check_stat_interrupts(ppu);
            }
            //ppu_check_stat_interrupts(ppu);
            break;
    }
}