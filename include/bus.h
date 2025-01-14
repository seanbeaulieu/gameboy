#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <stdio.h>

typedef struct bus {
    uint8_t *memory;
    uint8_t dpad_state;    // store dpad in bits 0-3
    uint8_t button_state;  // store buttons in bits 0-3 
    uint8_t joypad_select; // select bits
    
    // mbc (only no mbc and mbc3 handled)
    uint8_t *rom_data;    // full ROM data
    uint8_t mbc_type;     // 0=none, 1=mbc1, 2=mbc2, 3=mbc3 etc
    uint8_t rom_bank;     // current ROM bank
    uint8_t ram_bank;     // current RAM bank
    uint8_t ram_enabled;     // RAM access enabled

    // no rtc for mb3 

} bus;

void bus_init(bus *bus);
void bus_free(bus *bus);
uint8_t bus_read8(bus *bus, uint16_t address);
void bus_write8(bus *bus, uint16_t address, uint8_t value);
uint16_t bus_read16(bus *bus, uint16_t address);
void bus_write16(bus *bus, uint16_t address, uint16_t value);
void bus_increment_div(bus *bus);
// uint8_t bus_read_interrupt_register(bus *bus, uint16_t address);
// void bus_write_interrupt_register(bus *bus, uint16_t address, uint8_t value);
// uint8_t bus_read_timer_register(bus *bus, uint16_t address);
// void bus_write_timer_register(bus *bus, uint16_t address, uint8_t value);
int load_rom(bus *bus, const char *rom_path);
void print_bits(uint8_t value, const char *name);

#endif