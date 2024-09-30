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

uint8_t bus_read8(bus *bus, uint16_t address) {
    // return bus->memory[address];
    uint8_t value = bus->memory[address];
    // printf("Reading from address 0x%04X: 0x%02X\n", address, value);
    return value;
}

void bus_write8(bus *bus, uint16_t address, uint8_t value) {
    bus->memory[address] = value;
}

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

    // check if file exists
    if (file == NULL) {
        printf("Failed to open ROM file: %s\n", rom_path);
        return -1;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    // print the size of ROM
    printf("ROM file size: %ld bytes\n", file_size);

    // check if the size is larger than 64kb
    if (file_size > 65536) {
        printf("ROM file is too large\n");
        fclose(file);
        return -1;
    }

    // read ROM direct into bus memory
    size_t bytes_read = fread(bus->memory, 1, file_size, file);
    printf("bytes read %zu\n", bytes_read);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Failed to read ROM data. Read %zu bytes out of %ld\n", bytes_read, file_size);
        fclose(file);
        return -1;
    }

    printf("ROM loaded successfully. First byte: 0x%02X, Last byte: 0x%02X\n", 
           bus->memory[0], bus->memory[bytes_read - 1]);

    // Print the first 16 bytes of the ROM
    printf("First 16 bytes of ROM:\n");
    for (int i = 0; i < 16 && i < file_size; i++) {
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