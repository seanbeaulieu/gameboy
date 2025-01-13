#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include "../include/instruction.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ZERO_FLAG_BYTE_POSITION 7
#define SUBTRACT_FLAG_BYTE_POSITION 6
#define HALF_CARRY_FLAG_BYTE_POSITION 5
#define CARRY_FLAG_BYTE_POSITION 4

// initialize cpu registers
// todo
void cpu_init(cpu *cpu, ppu *ppu) {
    memset(&cpu->registers, 0, sizeof(cpu_registers));
    cpu->ppu = ppu;
}

// test init to set to boot rom at 0x100
void cpu_init_test(cpu_registers *registers) {
    registers->a = 0x01;
    registers->f.zero = 1;
    registers->f.subtract = 0;  
    registers->f.half_carry = 0; 
    registers->f.carry = 0;     
    registers->b = 0x00;
    registers->c = 0x13;
    registers->d = 0x00;
    registers->e = 0xD8;
    registers->h = 0x01;
    registers->l = 0x4D;
    registers->sp = 0xFFFE;
    registers->pc = 0x0100;
}

void cpu_init_test2(cpu_registers *registers) {
    registers->a = 0x00;
    registers->f.zero = 0;
    registers->f.subtract = 0;  
    registers->f.half_carry = 0; 
    registers->f.carry = 0;     
    registers->b = 0x00;
    registers->c = 0x00;
    registers->d = 0x00;
    registers->e = 0x00;
    registers->h = 0x00;
    registers->l = 0x00;
    registers->sp = 0x00;
    registers->pc = 0x0000;
}

// use switch here?
uint16_t cpu_read_register_16bit(cpu_registers *registers, const char *reg) {
    if (strcmp(reg, "af") == 0)
        return (uint16_t)((registers->a << 8) | flags_register_to_byte(registers->f));
    else if (strcmp(reg, "bc") == 0)
        return (uint16_t)((registers->b << 8) | registers->c);
    else if (strcmp(reg, "de") == 0)
        return (uint16_t)((registers->d << 8) | registers->e);
    else if (strcmp(reg, "hl") == 0)
        return (uint16_t)((registers->h << 8) | registers->l);
    else
        return 0;
}

// 16 bit write function
void cpu_write_register_16bit(cpu_registers *registers, const char *reg, uint16_t value) {
    if (strcmp(reg, "af") == 0) {
        registers->a = (value >> 8) & 0xFF;
        registers->f = byte_to_flags_register(value & 0xFF);
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

void cpu_increment_register_16bit(cpu_registers *registers, const char *reg) {
    uint16_t value = cpu_read_register_16bit(registers, reg);
    value++;
    cpu_write_register_16bit(registers, reg, value);
}

void cpu_decrement_register_16bit(cpu_registers *registers, const char *reg) {
    uint16_t value = cpu_read_register_16bit(registers, reg);
    value--;
    cpu_write_register_16bit(registers, reg, value);
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

// handle interrupts and timer 

void cpu_handle_interrupts(cpu *cpu) {
    uint8_t ie = bus_read8(&cpu->bus, 0xFFFF); // Interrupt Enable (IE)
    // if (ie) {
    //     // printf("ie is true ");
    // }
    uint8_t if_ = bus_read8(&cpu->bus, 0xFF0F); // Interrupt Flag (IF)
    // if (if_) {
    //     printf("if is true ");
    // }
    // printf("raw values: IE=%02X IF=%02X at %04X\n", ie, if_, 0xFF0F);
    uint8_t requested = ie & if_;
    // if (requested) {
    //     printf("requested is true ");
    // }
    // printf("handling interrupt - IE: 0x%02X, IF: 0x%02X\n", ie, if_);

    // cpu->halted = 0;
    cpu->ime = 0;

    if (requested & 0x01) {  // Vblank
        // printf("handling vblank interrupt\n");
        bus_write8(&cpu->bus, 0xFF0F, if_ & ~0x01); 
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);

        //cpu->ime = 0;
        // cpu->halted = 0;
        // set PC to the Vblank interrupt handler address
        cpu->registers.pc = 0x0040;
    } else if (requested & 0x02) {  // LCD status
        printf("handling stat interrupt\n");
        // printf("handling STAT interrupt, PC was: %04X\n", cpu->registers.pc);
        // printf("writing to IF at addr=%04X clearing bit 1\n", 0xFF0F);
        // uint16_t if_addr = 0xFF0F;
        // // printf("about to write to IF: addr=0x%04X\n", if_addr);
        // bus_write8(&cpu->bus, if_addr, if_ & ~0x02);
        bus_write8(&cpu->bus, 0xFF0F, if_ & ~0x02); 
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);

        //cpu->ime = 0;
        // cpu->halted = 0;
        // set PC to the LCD Status interrupt handler address
        cpu->registers.pc = 0x0048;
    } else if (requested & 0x04) {  // timer overflow
        // printf("handling tima interrupt\n");
        bus_write8(&cpu->bus, 0xFF0F, if_ & ~0x04); 
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
        
        //cpu->ime = 0;
        // cpu->halted = 0;
        // set PC to the Timer Overflow interrupt handler address
        cpu->registers.pc = 0x0050;
    } else if (requested & 0x08) {  // serial link
        printf("handling serial link interrupt\n");
        bus_write8(&cpu->bus, 0xFF0F, if_ & ~0x08); 
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
        
        //cpu->ime = 0;
        // cpu->halted = 0;
        // set PC to the Serial Link interrupt handler address
        cpu->registers.pc = 0x0058;
    } else if (requested & 0x10) {  // joypad press
        printf("handling joypad press interrupt\n");
        bus_write8(&cpu->bus, 0xFF0F, if_ & ~0x10);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
      
        // cpu->ime = 0;
        // cpu->halted = 0;
        // set PC to the Joypad Press interrupt handler address
        cpu->registers.pc = 0x0060;
    }
}


// update timers
// https://gbdev.io/pandocs/Timer_and_Divider_Registers.html
void cpu_update_timers(cpu *cpu) {
    // increment the DIV register at a fixed rate of 16384 Hz
    if (cpu->count % 256 == 0) {
        // printf("div timer incremented ");
        bus_increment_div(&cpu->bus);
    }

    // check if the timer is enabled (TAC bit 2)
    uint8_t tac = bus_read8(&cpu->bus, 0xFF07);
    // printf("tac & 0x04: %d\n", (tac & 0x04));
    if (tac & 0x04) {
        // determine the timer frequency based on TAC bits 0-1
        uint32_t cycles_per_increment;
        switch (tac & 0x03) {
            case 0x00: cycles_per_increment = 1024; break; // 4096 Hz 
            case 0x01: cycles_per_increment = 16; break;   // 262144 Hz 
            case 0x02: cycles_per_increment = 64; break;   // 65536 Hz 
            case 0x03: cycles_per_increment = 256; break;  // 16384 Hz 
        }
        if (cpu->count % cycles_per_increment == 0) {
            uint8_t tima = bus_read8(&cpu->bus, 0xFF05);
            tima++; // increment first
            // printf("%d", tima);
            // check for overflow after increment
            if (tima == 0) { // if it wrapped around to 0
                // printf("tima overflow");
                // set the timer interrupt flag
                uint8_t if_ = bus_read8(&cpu->bus, 0xFF0F);
                bus_write8(&cpu->bus, 0xFF0F, if_ | 0x04);

                // reset TIMA to the value in TMA
                tima = bus_read8(&cpu->bus, 0xFF06);
            }
            
            // write the new value back to TIMA
            bus_write8(&cpu->bus, 0xFF05, tima);
        }
    }
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
   
   // check for interrupts
    uint8_t ie = bus_read8(&cpu->bus, 0xFFFF); // interrupt Enable
    uint8_t if_ = bus_read8(&cpu->bus, 0xFF0F); // interrupt Flag

    if ((cpu->ime || cpu->halted) && (ie & if_ & 0x1F)) {
        cpu->halted = 0;
        if (cpu->ime) {
            cpu_handle_interrupts(cpu);
        }
    }

    // fetch the next instruction
    uint8_t opcode = bus_read8(&cpu->bus, cpu->registers.pc++);
    
    // set the counter for this instruction
    cpu->counter = op_tcycles[opcode]; 
    // convert T-cycles to M-cycles
    uint8_t m_cycles = op_tcycles[opcode] / 4;

    // tick ppu once for each M-cycle in the opcode
    for (int i = 0; i < m_cycles; i++) {
        ppu_step(cpu->ppu);
    }

    // step PPU only once per cpu step? and catch PPU timer up per T-cycle
    // cpu->ppu->dot_counter += op_tcycles[opcode];
    // ppu_step(cpu->ppu);
    // printf("counter: %d \n", cpu->counter);

    // execute the instruction
    // cb instructions are handled in instruction.c, counter and increment are done in 0xCB
    instruction_execute(cpu, opcode);

    // call update timers
    cpu_update_timers(cpu);

    // update the total cycle count
    cpu->count += cpu->counter;

    // reset the counter
    cpu->counter = 0;

}
