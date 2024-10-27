#ifndef PPU_H
#define PPU_H

#include <bus.h>
#include <cpu.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    uint8_t screen_buffer[23040];

    uint8_t mode;
    uint8_t current_ly;
    uint32_t dot_counter;
    uint8_t stat_irq_blocked;

    uint8_t sprite_buffer[10][4];  // max 10 sprites per scanline, 4 bytes each
    uint8_t sprite_count;

    uint8_t *vram;
    uint8_t *oam;
    bus *bus;

} ppu;

void ppu_init(ppu *ppu, bus *bus);
void ppu_step(ppu *ppu);
void ppu_cleanup(ppu *ppu);

// register functions
uint8_t ppu_read_register(ppu *ppu, uint16_t address);
void ppu_write_register(ppu *ppu, uint16_t address, uint8_t value);

// function for each ppu mode
void ppu_mode_oam_scan(ppu *ppu);
void ppu_mode_drawing(ppu *ppu);
void ppu_mode_hblank(ppu *ppu);
void ppu_mode_vblank(ppu *ppu);

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