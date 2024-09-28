#include "../include/cpu.h"
#include "../include/bus.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    cpu gameboy;
    memset(&gameboy, 0, sizeof(cpu));

    cpu_init(&gameboy.registers);
    bus_init(&gameboy.bus);

    // load rom 
    if (argc < 2) {
        printf("Please provide the path to the ROM file.\n");
        return 1;
    }

    const char *rom_path = argv[1];
    if (load_rom(&gameboy.bus, rom_path) == 0) {
        printf("First 10 opcodes:\n");
        for (int i = 0; i < 30; i++) {
            uint8_t opcode = bus_read8(&gameboy.bus, i);
            printf("Address 0x%04X: 0x%02X\n", i, opcode);
        }
    } else {
        fprintf(stderr, "Failed to load ROM. Exiting.\n");
        return 1;
    }

    printf("before game loop");
    // game loop
    while (1) {
        // printf("before cpu step, in while\n");
        cpu_step(&gameboy);
        
        // etc

    }

    return 0;
}