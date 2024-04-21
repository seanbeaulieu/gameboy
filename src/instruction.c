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
                    // do not think i will need to write to bc
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [bc]
                case 0x2: 
                    // write to the absolute address present in BC, whatever is in the A register
                    bus_write8(&cpu->bus, &cpu->registers.bc, cpu->registers.a);
                    break;
                // inc bc
                case 0x3:  
                    // increment the register bc
                    cpu_increment_register_16bit(&cpu->registers, 'bc');
                    break;
                // inc b
                case 0x4:
                    cpu->registers.b++;
                    cpu->registers.f.zero = (cpu->registers.b == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.b & 0x0F) == 0x00);
                    break;
                // dec b
                case 0x5: 
                    cpu->registers.b--;
                    cpu->registers.f.zero = (cpu->registers.b == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.b & 0x0F) == 0x00); 
                    break;
                // ld b, n8
                case 0x6:
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // rlca
                case 0x7:
                    cpu->registers.a = (cpu->registers.a << 1) | (cpu->registers.a >> 7);
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = (cpu->registers.a & 0x01);
                    break;
                // ld [a16], sp
                // load from stack pointer
                case 0x8:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++) | bus_read8(&cpu->bus, cpu->registers.pc++) << 8; 
                    bus_write8(&cpu->bus, nn++, cpu->registers.sp & 0xFF);
                    bus_write8(&cpu->bus, nn, cpu->registers.sp >> 8);
                    break;
                // add hl, bc
                case 0x9:
                    uint32_t nnnn = cpu_read_register_16bit(&cpu->registers, 'hl') + cpu_read_register_16bit(&cpu->registers, 'bc');
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (nnnn ^ cpu_read_register_16bit(&cpu->registers, 'hl') ^ cpu_read_register_16bit(&cpu->registers, 'bc')) & 0x1000 ? 1 : 0;
                    cpu->registers.f.carry = (nnnn & 0xFFFF0000) ? 1 : 0;
                    cpu->registers.h = (nnnn & 0x0000FF00) >> 8;
                    cpu->registers.l = (nnnn & 0x000000FF);
                    break;
                // ld a, [bc]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'bc'));
                    break;
                // dec bc
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, 'bc');
                    break;
                // inc c
                case 0xC:  
                    cpu->registers.c++;
                    cpu->registers.f.zero = (cpu->registers.c == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.c & 0x0F) == 0x00);
                    break;
                // dec c
                case 0xD:
                    cpu->registers.c--;
                    cpu->registers.f.zero = (cpu->registers.c == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.c & 0x0F) == 0x00); 
                    break;
                // ld c, n8
                case 0xE:
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // rrca
                case 0xF:  
                    cpu->registers.f.carry = cpu->registers.a & 0x01;
                    cpu->registers.a = (cpu->registers.a >> 1) | (cpu->registers.a << 7);
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
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