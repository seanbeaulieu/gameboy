#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct FlagsRegister {
    bool zero;
    bool subtract;
    bool half_carry;
    bool carry;
} FlagsRegister;

typedef struct cpu_registers {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    FlagsRegister f;
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
} cpu_registers;

void cpu_init(cpu_registers *registers);
uint16_t cpu_read_register_16bit(cpu_registers *registers, const char *reg);
void cpu_write_register_16bit(cpu_registers *registers, const char *reg, uint16_t value);
uint8_t flags_register_to_byte(FlagsRegister flags);
FlagsRegister byte_to_flags_register(uint8_t byte);

#endif