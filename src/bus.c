#include "../include/bus.h"
#include <stdlib.h>
#include <stdio.h>


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

// interrupts and timer

uint8_t bus_read_interrupt_register(bus *bus, uint16_t address) {
    switch (address) {
        case 0xFF0F: // Interrupt Flag (IF)
            return bus->memory[0xFF0F];
        case 0xFFFF: // Interrupt Enable (IE)
            return bus->memory[0xFFFF];
        default:
            return 0;
    }
}

void bus_write_interrupt_register(bus *bus, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xFF0F: // Interrupt Flag (IF)
            bus->memory[0xFF0F] = value;
            break;
        case 0xFFFF: // Interrupt Enable (IE)
            bus->memory[0xFFFF] = value;
            break;
    }
}

uint8_t bus_read_timer_register(bus *bus, uint16_t address) {
    switch (address) {
        case 0xFF04: // DIV Register
            return bus->memory[0xFF04];
        case 0xFF05: // TIMA Register
            return bus->memory[0xFF05];
        case 0xFF06: // TMA Register
            return bus->memory[0xFF06];
        case 0xFF07: // TAC Register
            return bus->memory[0xFF07];
        default:
            return 0;
    }
}

void bus_write_timer_register(bus *bus, uint16_t address, uint8_t value) {
    switch (address) {
        case 0xFF04: // DIV Register
            bus->memory[0xFF04] = 0; // Writing any value resets DIV to 0
            break;
        case 0xFF05: // TIMA Register
            bus->memory[0xFF05] = value;
            break;
        case 0xFF06: // TMA Register
            bus->memory[0xFF06] = value;
            break;
        case 0xFF07: // TAC Register
            bus->memory[0xFF07] = value;
            break;
    }
}

// load ROM memory

int load_rom(bus *bus, const char *rom_path) {
    FILE *file = fopen(rom_path, "rb");

    // check if file exists
    if (file == NULL) {
        printf("Failed to open ROM file: %s\n", rom_path);
        return -1;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // allocate memory for the ROM data
    uint8_t *rom_data = (uint8_t *)malloc(file_size);
    if (rom_data == NULL) {
        printf("Failed to allocate memory for ROM data\n");
        fclose(file);
        return -1;
    }

    // read the ROM data from the file
    size_t bytes_read = fread(rom_data, 1, file_size, file);
    if (bytes_read != file_size) {
        printf("Failed to read ROM data from file\n");
        free(rom_data);
        fclose(file);
        return -1;
    }

    // copy the ROM data to the appropriate memory locations in the bus
    memcpy(bus->memory, rom_data, file_size);

    free(rom_data);
    fclose(file);
    return 0;
}