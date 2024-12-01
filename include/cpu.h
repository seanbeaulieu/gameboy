#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <bus.h>
#include <ppu.h>
#include <stdio.h>

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
    uint16_t af;
    uint16_t bc;
    uint16_t de;
    uint16_t hl;
    uint16_t pc;
    uint16_t sp;
} cpu_registers;

typedef struct cpu {
    cpu_registers registers;
    bus bus;
    ppu *ppu;
    uint8_t counter;
    bool ime; // interrupt
    uint8_t halted;
    uint32_t count; // clock
} cpu;


void cpu_init(cpu *cpu, ppu *ppu);
void cpu_init_test(cpu_registers *registers);
uint16_t cpu_read_register_16bit(cpu_registers *registers, const char *reg);
void cpu_write_register_16bit(cpu_registers *registers, const char *reg, uint16_t value);
void cpu_increment_register_16bit(cpu_registers *registers, const char *reg);
void cpu_decrement_register_16bit(cpu_registers *registers, const char *reg);
uint8_t flags_register_to_byte(FlagsRegister flags);
FlagsRegister byte_to_flags_register(uint8_t byte);

void cpu_handle_interrupts(cpu *cpu);
void cpu_update_timers(cpu *cpu);

void cpu_step(cpu *cpu);

// bus



#endif