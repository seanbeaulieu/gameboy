#include "../include/bus.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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
    bus->memory = (uint8_t *)malloc(65536);
    if (bus->memory == NULL) {
        fprintf(stderr, "Failed to allocate memory for bus\n");
        exit(1);
    }
    // initialize the memory to a known state
    memset(bus->memory, 0, 65536);

    // print check to see if memory properly intialized to load rom into
    printf("Bus memory initialized. First byte: 0x%02X, Last byte: 0x%02X\n", 
    bus->memory[0], bus->memory[65535]);
}

// free bus memory
void bus_free(bus *bus) {
    if (bus->memory != NULL) {
        free(bus->memory);
        bus->memory = NULL;
    }
}

uint8_t bus_read8(bus *bus, uint16_t address) {
    // for testing
    if (address == 0xFF44) {
        return 0x90;  
    }
    
    
    return bus->memory[address];
}

void bus_write8(bus *bus, uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        // ROM - typically not writable
        // Maybe handle bank switching here
    } else if (address < 0xA000) {
        // VRAM
        bus->memory[address] = value;
        // Maybe trigger some graphics update
    } else if (address < 0xC000) {
        // External RAM
        printf("writing to WRAM");
        bus->memory[address] = value;
    } else if (address < 0xE000) {
        // WRAM
        bus->memory[address] = value;
        // Mirror to Echo RAM
        if (address < 0xDE00) {
            bus->memory[address + 0x2000] = value;
        }
    } else if (address < 0xFE00) {
        // Echo RAM - write to WRAM instead
        bus->memory[address - 0x2000] = value;
    } else if (address < 0xFEA0) {
        // OAM
        bus->memory[address] = value;
        // Maybe trigger sprite update
    } else if (address < 0xFF00) {
        // Not usable
    } else if (address < 0xFF80) {
        // I/O Registers
        // Handle special I/O behavior here
    } else {
        // High RAM (HRAM)
        bus->memory[address] = value;
    }
}


// review this and any instructions that call it
uint16_t bus_read16(bus *bus, uint16_t address) {
    return bus_read8(bus, address) | (bus_read8(bus, address + 1) << 8);
}

void bus_write16(bus *bus, uint16_t address, uint16_t value) {
    bus_write8(bus, address, value & 0xFF);
    bus_write8(bus, address + 1, value >> 8);
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

    if (file == NULL) {
        printf("Failed to open ROM file: %s\n", rom_path);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    printf("ROM file size: %ld bytes\n", file_size);

    // Determine how much of the ROM to load
    size_t rom_size = (file_size > 0x8000) ? 0x8000 : file_size;

    // Read ROM into the correct memory region (0x0000 - 0x7FFF)
    size_t bytes_read = fread(bus->memory, 1, rom_size, file);
    
    if (bytes_read != rom_size) {
        fprintf(stderr, "Failed to read ROM data. Read %zu bytes out of %zu\n", bytes_read, rom_size);
        fclose(file);
        return -1;
    }

    printf("ROM loaded successfully. First byte: 0x%02X, Last byte: 0x%02X\n", 
           bus->memory[0], bus->memory[bytes_read - 1]);

    // Print the first 16 bytes of the ROM
    printf("First 16 bytes of ROM:\n");
    for (int i = 0; i < 16 && i < rom_size; i++) {
        printf("%02X ", bus->memory[i]);
        if ((i + 1) % 8 == 0) printf("\n");
    }
    printf("\n");

    printf("ROM Title: %.16s\n", &bus->memory[0x134]);
    printf("Cartridge Type: 0x%02X\n", bus->memory[0x147]);
    printf("ROM Size: 0x%02X\n", bus->memory[0x148]);
    printf("RAM Size: 0x%02X\n", bus->memory[0x149]);

    fclose(file);
    return 0;
}