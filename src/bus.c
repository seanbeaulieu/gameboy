#include "../include/bus.h"
#include <stdlib.h>

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

void bus_init(bus *bus) {
    // allocate 64KB of memory
    bus->memory = (uint8_t *)malloc(65536); 
    // need to intialize other components
}

uint8_t bus_read8(bus *bus, uint16_t address) {
    return bus->memory[address];
}

void bus_write8(bus *bus, uint16_t address, uint8_t value) {
    bus->memory[address] = value;
}