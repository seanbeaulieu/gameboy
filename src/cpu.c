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
        return (registers->a << 8) | flags_register_to_byte(byte_to_flags_register(&registers->f));
    else if (strcmp(reg, "bc") == 0)
        return registers->bc;
    else if (strcmp(reg, "de") == 0)
        return registers->de;
    else if (strcmp(reg, "hl") == 0)
        return registers->hl;
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
        registers->f = flags_register_to_byte(byte_to_flags_register(value & 0xFF));
    } else if (strcmp(reg, "bc") == 0) {
        registers->bc = value;
    } else if (strcmp(reg, "de") == 0) {
        registers->de = value;
    } else if (strcmp(reg, "hl") == 0) {
        registers->hl = value;
    } else if (strcmp(reg, "pc") == 0) {
        registers->pc = value;
    } else if (strcmp(reg, "sp") == 0) {
        registers->sp = value;
    }
}

void cpu_increment_register_16bit(cpu_registers *registers, const char *reg) {
    if (strcmp(reg, "bc") == 0) {
        uint16_t nn = registers->bc + 1;
        registers->bc = nn;
    } else if (strcmp(reg, "de") == 0) {
        uint16_t nn = registers->de + 1;
        registers->de = nn;
    } else if (strcmp(reg, "hl") == 0) {
        uint16_t nn = registers->hl + 1;
        registers->hl = nn;
    } else if (strcmp(reg, "sp") == 0) {
        uint16_t nn = registers->sp + 1;
        registers->sp = nn;
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

// T-cycles for opcodes
// from greg tourville

uint8_t op_tcycles[0x100] = {
	//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	4,12, 8, 8, 4, 4, 8, 4,20, 8, 8, 8, 4, 4, 8, 4,    // 0x00
	4,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,    // 0x10
	8,12, 8, 8, 4, 4, 8, 4, 8, 8, 8, 8, 4, 4, 8, 4,    // 0x20
	8,12, 8, 8,12,12,12, 4, 8, 8, 8, 8, 4, 4, 8, 4,    // 0x30
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x40
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x50
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x60
	8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x70
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x80
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0x90
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0xA0
	4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,    // 0xB0
	8,12,12,12,12,16, 8,32, 8, 8,12, 8,12,12, 8,32,    // 0xC0
	8,12,12, 0,12,16, 8,32, 8, 8,12, 0,12, 0, 8,32,    // 0xD0
	12,12, 8, 0, 0,16, 8,32,16, 4,16, 0, 0, 0, 8,32,    // 0xE0
	12,12, 8, 4, 0,16, 8,32,12, 8,16, 4, 0, 0, 8,32     // 0xF0
};

// step function

void cpu_step(cpu *cpu) {
    // fetch the instruction byte at the current program counter
    uint8_t opcode = bus_read8(&cpu->bus, cpu->registers.pc);
    
    // figure out cycle counter

    // Counter-=Cycles[OpCode];
    // convert to M cycles
    cpu->counter = op_tcycles[opcode] / 4;

    // translate opcode into instruction
    // execute opcode
    instruction_execute(cpu, opcode);

    if (cpu->counter <= 0) {
        // interrupts, cyclic tasks

        // reset/increment cpu counter 
        // if(ExitRequired) break;

    }
    
    // increment the program counter
    cpu->registers.pc++;
}