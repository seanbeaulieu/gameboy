#include "../include/instruction.h"
#include "../include/cpu.h"

void instruction_execute(cpu *cpu, uint8_t opcode) {
    switch (opcode & 0xF0) {
        case 0x00:
            switch (opcode & 0x0F) {
                // nop
                case 0x0:  
                    // do nothing
                    break;
                // ld bc 
                case 0x1:
                    // little or big endian??
                    // also when to read and write to bc and not just b and c? 
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.pc++);

                    break;
                // ld (bc)
                case 0x2: 
                    // write to the absolute address present in BC, whatever is in the A register
                    bus_write8(&cpu->bus, &cpu->registers.bc, cpu->registers.a);
                    break;
                // inc bc
                case 0x3:  
                    // increment the register bc
                    cpu_increment_register_16bit(&cpu->registers, 'bc');
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;




        case 0x10:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0x20:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0x30:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0x40:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0x50:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0x60:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0x70:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0x80:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0x90:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0xA0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0xB0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0xC0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                
                // prefix
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0xD0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;



        case 0xE0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
            break;



        
        case 0xF0:
            switch (opcode & 0x0F) {
                case 0x0:  
                    break;
                case 0x1:  
                    break;
                case 0x2:  
                    break;
                case 0x3:  
                    break;
                case 0x4:  
                    break;
                case 0x5:  
                    break;
                case 0x6:  
                    break;
                case 0x7:  
                    break;
                case 0x8:  
                    break;
                case 0x9:  
                    break;
                case 0xA:  
                    break;
                case 0xB:  
                    break;
                case 0xC:  
                    break;
                case 0xD:  
                    break;
                case 0xE:  
                    break;
                case 0xF:  
                    break;
            }
        break;

        

        
        default:
            
            break;
    }
}