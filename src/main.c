#include "../include/cpu.h"
#include "../include/bus.h"
#include <stdio.h>

int main() {
    cpu gameboy;
    memset(&gameboy, 0, sizeof(cpu));

    cpu_init(&gameboy.registers);
    bus_init(&gameboy.bus);

    // load rom 
    
    // game loop
    while (1) {
        cpu_step(&gameboy);

        // etc
        
    }

    return 0;
}