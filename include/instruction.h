#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <cpu.h>

typedef struct instruction {
    uint8_t opcode;
    void (*execute)(cpu *cpu);
} instruction;

void instruction_execute(cpu *cpu, uint8_t opcode);

#endif

