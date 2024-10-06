#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include <cpu.h>

typedef struct prefix_instruction {
    uint8_t opcode;
    void (*execute)(cpu *cpu);
} orefix_instruction;

void prefix_instruction_execute(cpu *cpu, uint8_t opcode);
static uint8_t* get_register_pointer(cpu *cpu, uint8_t reg_index);

#endif
