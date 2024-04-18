#include "../include/cpu.h"
#include "../include/bus.h"
#include <string.h>

#define ZERO_FLAG_BYTE_POSITION 7
#define SUBTRACT_FLAG_BYTE_POSITION 6
#define HALF_CARRY_FLAG_BYTE_POSITION 5
#define CARRY_FLAG_BYTE_POSITION 4

// initialize cpu registers
// 
void cpu_init(cpu_registers *registers) {
    memset(registers, 0, sizeof(cpu_registers));
}

// use switch here?
uint16_t cpu_read_register_16bit(cpu_registers *registers, const char *reg) {
    if (strcmp(reg, "af") == 0)
        return (registers->a << 8) | flags_register_to_byte(registers->f);
    else if (strcmp(reg, "bc") == 0)
        return (registers->b << 8) | registers->c;
    else if (strcmp(reg, "de") == 0)
        return (registers->d << 8) | registers->e;
    else if (strcmp(reg, "hl") == 0)
        return (registers->h << 8) | registers->l;
    else if (strcmp(reg, "pc") == 0)
        return registers->pc;
    else if (strcmp(reg, "sp") == 0)
        return registers->sp;
    else
        return 0;
}

void cpu_write_register_16bit(cpu_registers *registers, const char *reg, uint16_t value) {
    if (strcmp(reg, "af") == 0) {
        registers->a = (value >> 8) & 0xFF;
        registers->f = byte_to_flags_register(value & 0xF0);
    } else if (strcmp(reg, "bc") == 0) {
        registers->b = (value >> 8) & 0xFF;
        registers->c = value & 0xFF;
    } else if (strcmp(reg, "de") == 0) {
        registers->d = (value >> 8) & 0xFF;
        registers->e = value & 0xFF;
    } else if (strcmp(reg, "hl") == 0) {
        registers->h = (value >> 8) & 0xFF;
        registers->l = value & 0xFF;
    } else if (strcmp(reg, "pc") == 0) {
        registers->pc = value;
    } else if (strcmp(reg, "sp") == 0) {
        registers->sp = value;
    }
}

uint8_t flags_register_to_byte(FlagsRegister flags) {
    return (flags.zero << ZERO_FLAG_BYTE_POSITION) |
           (flags.subtract << SUBTRACT_FLAG_BYTE_POSITION) |
           (flags.half_carry << HALF_CARRY_FLAG_BYTE_POSITION) |
           (flags.carry << CARRY_FLAG_BYTE_POSITION);
}

FlagsRegister byte_to_flags_register(uint8_t byte) {
    // define a flags struct, and then set each flag to it's bit value
    FlagsRegister flags;
    flags.zero = (byte >> ZERO_FLAG_BYTE_POSITION) & 0x01;
    flags.subtract = (byte >> SUBTRACT_FLAG_BYTE_POSITION) & 0x01;
    flags.half_carry = (byte >> HALF_CARRY_FLAG_BYTE_POSITION) & 0x01;
    flags.carry = (byte >> CARRY_FLAG_BYTE_POSITION) & 0x01;
    return flags;
}


// Step function

void cpu_step(cpu *cpu) {
    // fetch the instruction byte at the current program counter
    uint8_t opcode = bus_read8(&cpu->bus, cpu->registers.pc);
    
    // translate opcode into instruction
    // execute opcode
    instruction_execute(cpu, opcode);
    
    // increment the program counter
    cpu->registers.pc++;
}