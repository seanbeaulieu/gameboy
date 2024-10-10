#include "../include/cpu.h"
#include "../include/bus.h"
#include <stdio.h>
#include <string.h>

FILE *log_file = NULL;
#define MAX_CYCLES 10000000

void debug_print(cpu *gameboy) {
    fprintf(log_file, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
           gameboy->registers.a,
           flags_register_to_byte(gameboy->registers.f),
           gameboy->registers.b,
           gameboy->registers.c,
           gameboy->registers.d,
           gameboy->registers.e,
           gameboy->registers.h,
           gameboy->registers.l,
           gameboy->registers.sp,
           gameboy->registers.pc,
           bus_read8(&gameboy->bus, gameboy->registers.pc),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 1),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 2),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 3));
}

int main(int argc, char *argv[]) {
    cpu gameboy;
    memset(&gameboy, 0, sizeof(cpu));

    cpu_init(&gameboy.registers);
    cpu_init_test(&gameboy.registers);
    bus_init(&gameboy.bus);

    // open log file
    log_file = fopen("logfile.txt", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Failed to open log file. Exiting.\n");
        return 1;
    }

    // load rom 
    if (argc < 2) {
        printf("Please provide the path to the ROM file.\n");
        return 1;
    }

    const char *rom_path = argv[1];
    if (load_rom(&gameboy.bus, rom_path) == 0) {
        
        // printf("First 30 opcodes:\n");
        // for (int i = 0; i < 400; i++) {
        //     uint8_t opcode = gameboy.bus.memory[i];
        //     //printf("Address 0x%04X: 0x%02X\n", i, opcode);
        // }
    } else {
        fprintf(stderr, "Failed to load ROM. Exiting.\n");
        return 1;
    }

    printf("before game loop\n");
    // game loop

    // set pc to 0x100
    ////////////////////////////////
    gameboy.registers.pc = 0x100;
    ////////////////////////////////
    int index = 0;
    while (index < 7500000) {
        // printf("before cpu step, in while\n");

        // debug_print(&gameboy);

        cpu_step(&gameboy);

        index++;
        // etc

        // if (gameboy.bus.memory[0xff02] == 0x81) {
        //     char c = gameboy.bus.memory[0xff01];
        //     printf("%c", c);
        //     gameboy.bus.memory[0xff02] = 0x0;
        // }

    }
    bus_free(&gameboy.bus);
    fclose(log_file);
    return 0;
}