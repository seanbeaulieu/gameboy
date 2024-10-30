#ifndef PPU_H
#define PPU_H

#include <bus.h>
#include <cpu.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// screen dimensions
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define MAX_SPRITES_PER_LINE 10

typedef struct {
    uint8_t y_pos;      // y position on screen (stored value + 16)
    uint8_t x_pos;      // x position on screen (stored value + 8)
    uint8_t tile_num;   // tile number in vram
    uint8_t flags;      // sprite flags (priority, flip, palette)
    uint8_t index;      // original oam index
} sprite_data;

typedef struct {
    // uint8_t screen_buffer[23040];

    uint8_t mode;
    uint8_t current_ly;
    uint32_t dot_counter; 
    uint8_t stat_irq_blocked;

    uint8_t sprite_count;
    sprite_data sprite_buffer[MAX_SPRITES_PER_LINE];  // buffer for current scanline sprites

    uint8_t screen_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

    // callback
    void (*frame_complete_callback)(uint8_t *buffer);

    uint8_t *vram;
    uint8_t *oam;
    bus *bus;

} ppu;

void ppu_init(ppu *ppu, bus *bus);
void ppu_step(ppu *ppu);
// void ppu_cleanup(ppu *ppu);
void ppu_set_frame_callback(ppu *ppu, void (*callback)(uint8_t *buffer));

// register functions
// uint8_t ppu_read_register(ppu *ppu, uint16_t address);
void ppu_write_register(ppu *ppu, uint16_t address, uint8_t value);

// functions for ppu modes
void ppu_oam_scan(ppu *ppu);
void ppu_draw(ppu *ppu);
// void ppu_hblank(ppu *ppu);
// void ppu_vblank(ppu *ppu);

// interrupts
void ppu_check_lyc(ppu *ppu);
void ppu_check_stat_interrupt(ppu *ppu);
void ppu_update_stat(ppu *ppu);

// tile and sprite rendering
void render_scanline(ppu *ppu);
void render_background(ppu *ppu);
void render_window(ppu *ppu);
void render_sprites(ppu *ppu);

// helper functions
uint8_t get_tile_pixel(uint8_t *tile_data, uint8_t x, uint8_t y);
uint8_t get_color_from_palette(uint8_t color_num, uint8_t palette);
uint16_t get_tile_data_address(ppu *ppu, uint8_t tile_number);

#endif