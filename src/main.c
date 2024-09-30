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
        printf("First 30 opcodes:\n");
        for (int i = 0; i < 400; i++) {
            uint8_t opcode = gameboy.bus.memory[i];
            printf("Address 0x%04X: 0x%02X\n", i, opcode);
        }
    } else {
        fprintf(stderr, "Failed to load ROM. Exiting.\n");
        return 1;
    }

    printf("before game loop");
    // game loop

    // set pc to 0x100
    ////////////////////////////////
    gameboy.registers.pc = 0x100;
    ////////////////////////////////

    while (1) {
        // printf("before cpu step, in while\n");
        cpu_step(&gameboy);
        
        // etc

        //uint8_t test1 = gameboy.bus.memory[0xFF02];
        //printf("Value at 0xFF02 (SC): 0x%02X\n", test1);
        if (gameboy.bus.memory[0xff02] == 0x81) {
            char c = gameboy.bus.memory[0xff01];
            printf("%c", c);
            gameboy.bus.memory[0xff02] = 0x0;
        }

    }

    return 0;
}