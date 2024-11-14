#include "../include/prefix_instruction.h"
#include "../include/cpu.h"

// helper function to get register pointer
static uint8_t* get_register_pointer(cpu *cpu, uint8_t reg_index) {
    switch (reg_index) {
        case 0: return &cpu->registers.b;
        case 1: return &cpu->registers.c;
        case 2: return &cpu->registers.d;
        case 3: return &cpu->registers.e;
        case 4: return &cpu->registers.h;
        case 5: return &cpu->registers.l;
        case 6: return NULL; 
        case 7: return &cpu->registers.a;
        default: return NULL; 
    }
}

void prefix_instruction_execute(cpu *cpu, uint8_t opcode) {
    uint8_t reg_index = opcode & 0x07;
    uint8_t bit_index = (opcode >> 3) & 0x07;
    uint8_t *reg_ptr;
    uint8_t value;

    // call helper to get register pointer
    reg_ptr = get_register_pointer(cpu, reg_index);

    // For (HL) operations
    if (reg_index == 0x06) {
        value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
    } else {
        value = *reg_ptr;
    }

    switch (opcode & 0xC0) {
        case 0x00: // Rotations and shifts
            switch (opcode & 0xF8) {
                case 0x00: // RLC
                    value = (value << 1) | (value >> 7);
                    cpu->registers.f.carry = (value & 0x01);
                    break;
                case 0x08: // RRC
                    cpu->registers.f.carry = value & 0x01;
                    value = (value >> 1) | (value << 7);
                    break;
                case 0x10: // RL
                    {
                        uint8_t old_carry = cpu->registers.f.carry;
                        cpu->registers.f.carry = (value & 0x80) >> 7;
                        value = (value << 1) | old_carry;
                    }
                    break;
                case 0x18: // RR
                    {
                        uint8_t old_carry = cpu->registers.f.carry;
                        cpu->registers.f.carry = value & 0x01;
                        value = (value >> 1) | (old_carry << 7);
                    }
                    break;
                case 0x20: // SLA
                    cpu->registers.f.carry = (value & 0x80) >> 7;
                    value <<= 1;
                    break;
                case 0x28: // SRA
                    cpu->registers.f.carry = value & 0x01;
                    value = (value & 0x80) | (value >> 1);
                    break;
                case 0x30: // SWAP
                    value = ((value & 0xF0) >> 4) | ((value & 0x0F) << 4);
                    cpu->registers.f.carry = 0;
                    break;
                case 0x38: // SRL
                    cpu->registers.f.carry = value & 0x01;
                    value >>= 1;
                    break;
            }
            cpu->registers.f.zero = (value == 0);
            cpu->registers.f.subtract = 0;
            cpu->registers.f.half_carry = 0;
            break;

        case 0x40: // BIT
            cpu->registers.f.zero = !(value & (1 << bit_index));
            cpu->registers.f.subtract = 0;
            cpu->registers.f.half_carry = 1;
            return; // don't write back for BIT operations

        case 0x80: // RES
            value &= ~(1 << bit_index);
            break;

        case 0xC0: // SET
            value |= (1 << bit_index);
            break;
    }

    // Write back the result
    if (reg_index == 0x06) {
        bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), value);
    } else {
        *reg_ptr = value;
    }
}