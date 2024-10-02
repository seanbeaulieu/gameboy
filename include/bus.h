#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <stdio.h>

typedef struct bus {
    uint8_t *memory;
    // etc
} bus;

void bus_init(bus *bus);
void bus_free(bus *bus);
uint8_t bus_read8(bus *bus, uint16_t address);
void bus_write8(bus *bus, uint16_t address, uint8_t value);
uint16_t bus_read16(bus *bus, uint16_t address);
void bus_write16(bus *bus, uint16_t address, uint16_t value);
uint8_t bus_read_interrupt_register(bus *bus, uint16_t address);
void bus_write_interrupt_register(bus *bus, uint16_t address, uint8_t value);
uint8_t bus_read_timer_register(bus *bus, uint16_t address);
void bus_write_timer_register(bus *bus, uint16_t address, uint8_t value);
int load_rom(bus *bus, const char *rom_path);

#endif