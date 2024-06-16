#include "../include/cpu.h"
#include "../include/bus.h"
#include <stdio.h>

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
    if (load_rom(&gameboy.bus, rom_path) != 0) {
        return 1;
    }


    // game loop
    while (1) {
        cpu_step(&gameboy);

        // etc

    }

    return 0;
}