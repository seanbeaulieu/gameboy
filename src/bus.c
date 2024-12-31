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
    bus->joypad_raw = 0xFF;  // all buttons released
    bus->joypad_select = 0xFF;  // nothing selected
    // print check to see if memory properly intialized to load rom into
    // printf("Bus memory initialized. First byte: 0x%02X, Last byte: 0x%02X\n", 
    // bus->memory[0], bus->memory[65535]);
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
    // if (address == 0xFF44) {
    //     return 0x90;  
    // }
    if (address < 0x8000) {
        // ROM - typically not writable
        // Maybe handle bank switching here
        return bus->memory[address];
    } else if (address < 0xA000) {
        // VRAM
        // we need to check the PPU mode here
        // only modes 0, 1, and 2 can access VRAM
        // any attempts to write are ignored 
        // any attmepts to read return 0xFF
        // read the STAT mode bit to determine the mode the PPU is currently on 
        // if it's anything besides drawing (3) , we'll return the read
        if ((bus->memory[0xFF41] & 0x03) != 3) {
            return bus->memory[address];
        } else {
            return 0xFF;
        }
        // maybe trigger some graphics update
    } else if (address == 0xFF00) {
        uint8_t result = bus->joypad_select & 0x30;  // get select bits
        
        // if dpad selected (bit 4 = 0)
        if (!(bus->joypad_select & 0x10)) {
            // printf("dpad selected in bus_read\n");
            result |= bus->joypad_raw & 0x0F;
        }
        // if buttons selected (bit 5 = 0)  
        else if (!(bus->joypad_select & 0x20)) {
            result |= bus->joypad_raw & 0x0F;
        }
        else {
            result |= 0x0F;  // nothing selected = all released
        }
        return result;
    }
    return bus->memory[address];
}

// void bus_write8(bus *bus, uint16_t address, uint8_t value) {
//     // for testing, treat all memory as RAM
//     bus->memory[address] = value;
// }

//////////////////////////////////////////////////////////////////////////////////////////////

void bus_write8(bus *bus, uint16_t address, uint8_t value) {
    if (address < 0x8000) {
        // ROM - typically not writable
        // Maybe handle bank switching here
        bus->memory[address] = value;
    } else if (address < 0xA000) {
        // VRAM
        // we need to check the PPU mode here
        // only modes 0, 1, and 2 can access VRAM
        // any attempts to write are ignored 
        if ((bus->memory[0xFF41] & 0x03) != 3) {
            bus->memory[address] = value;
        }

        // print where the write is occurring
        // if (address >= 0x8000 && address <= 0x9FFF) {
        //     if (address <= 0x97FF) {
        //         printf("VRAM write: tile data at 0x%04X = 0x%02X\n", address, value);
        //     } else {
        //         printf("VRAM write: tile map at 0x%04X = 0x%02X\n", address, value);
        //     }
        // }

        // write to VRAM if PPU is not in draw mode
        if ((bus->memory[0xFF41] & 0x03) != 3) {
            bus->memory[address] = value;
        }

        // maybe trigger some graphics update
    } else if (address < 0xC000) {
        // external RAM
        // printf("writing to WRAM");
        bus->memory[address] = value;
    } else if (address < 0xE000) {
        // WRAM
        bus->memory[address] = value;
        // mirror to echo RAM
        if (address < 0xDE00) {
            bus->memory[address + 0x2000] = value;
        }
    } else if (address < 0xFE00) {
        // echo RAM - write to WRAM instead
        bus->memory[address - 0x2000] = value;
    } else if (address < 0xFEA0) {
        // OAM
        bus->memory[address] = value;
        // maybe trigger sprite update
    } else if (address < 0xFF00) {
        // ?
    } else if (address < 0xFF80) {
        // I/O Registers
        // if (address == 0xFF01) {
        //     printf("serial write: 0x%02X ('%c')\n", value);

        // }

        // // input register
        if (address == 0xFF00) {
            bus->joypad_select = (value & 0x30) | 0xCF;  // only bits 4-5 writable
            return;
            }
        
        if (address == 0xFF0F || address == 0xFFFF || (address >= 0xFF04 && address <= 0xFF07)) {
            // interrupt and timer registers
            // DIV register
            if (address == 0xFF04) { 
                 // writing any value resets DIV to 0
                bus->memory[address] = 0;
            } else {
                bus->memory[address] = value;
            }
            return;
        }
        
        // if (address == 0xFF40) {
        //     // printf("writing to LCDC\n");
        //     // print_bits(value);
        //     bus->memory[address] = value;
        // }

        else if (address == 0xFF41) {
            // make exception for PPU writes
            // this is hacky but works for now
            if (value & 0x04 || !(value & 0x04)) {  // if setting or clearing bit 2
                bus->memory[address] = value;  // allow full write from PPU
            } else {
                // normal case - only bits 3-6 writable
                uint8_t current = bus_read8(bus, 0xFF41);
                uint8_t writable_bits = value & 0x78;
                uint8_t readonly_bits = current & 0x87;
                bus->memory[address] = writable_bits | readonly_bits;
            }
            return;
        }
        else {
            bus->memory[address] = value;
            return;
        }
    } else {
        // high RAM (HRAM)
        bus->memory[address] = value;
    }
    return;
}

/////////////////////////////////////////////////////////////////////////////

// review this and any instructions that call it
uint16_t bus_read16(bus *bus, uint16_t address) {
    return bus_read8(bus, address) | (bus_read8(bus, address + 1) << 8);
}

void bus_write16(bus *bus, uint16_t address, uint16_t value) {
    bus_write8(bus, address, value & 0xFF);
    bus_write8(bus, address + 1, value >> 8);
}

// interrupts and timer
// special to handle DIV increment outside of write 8
void bus_increment_div(bus *bus) {
    bus->memory[0xFF04]++;
}

// load ROM memory
// In each cartridge, the required (or preferred) MBC type should be specified in the byte at $0147 of the ROM, as described in the cartridge header.
// 

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

    // determine how much of the ROM to load
    size_t rom_size = (file_size > 0x8000) ? 0x8000 : file_size;

    // read ROM into the correct memory region (0x0000 - 0x7FFF)
    size_t bytes_read = fread(bus->memory, 1, rom_size, file);
    
    if (bytes_read != rom_size) {
        fprintf(stderr, "Failed to read ROM data. Read %zu bytes out of %zu\n", bytes_read, rom_size);
        fclose(file);
        return -1;
    }

    printf("ROM loaded successfully. First byte: 0x%02X, Last byte: 0x%02X\n", bus->memory[0], bus->memory[bytes_read - 1]);

    fclose(file);
    return 0;
}

// helper to print bits
void print_bits(uint8_t value) {
    printf("0x%02X (", value);
    for (int i = 7; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i == 4) printf(" ");
    }
    printf(")\n");
}