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
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.pc++);
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
                    cpu->registers.f.half_carry = ((cpu->registers.b & 0x0F) == 0x0F); 
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
                    cpu->registers.f.half_carry = ((cpu->registers.c & 0x0F) == 0x0F); 
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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x10:
            switch (opcode & 0x0F) {
                // halt 
                case 0x0:  
                    // set cpu to stop executing instructions
                    break;
                // ld de
                case 0x1:
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [de], a
                case 0x2:
                    bus_write8(&cpu->bus, &cpu->registers.de, cpu->registers.a);
                    break;
                // inc de
                case 0x3:
                    // increment the register de
                    cpu_increment_register_16bit(&cpu->registers, 'de');                   
                    break;
                // inc d
                case 0x4:
                    cpu->registers.d++;
                    cpu->registers.f.zero = (cpu->registers.d == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.d & 0x0F) == 0x00); 
                    break;
                // dec d
                case 0x5:
                    cpu->registers.d--;
                    cpu->registers.f.zero = (cpu->registers.d == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.d & 0x0F) == 0x0F); 
                    break;
                // ld d, n8
                case 0x6:
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.pc++);  
                    break;
                // rla
                case 0x7:
                    uint8_t n = cpu->registers.a;
                    cpu->registers.a = cpu->registers.a << 1 | cpu->registers.f.carry;
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = (n >> 7) & 0x01;
                    break;
                // jr, e8
                case 0x8:
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.pc += sn;
                    break;
                // add hl, de
                case 0x9:
                    uint32_t nnnn = cpu_read_register_16bit(&cpu->registers, 'hl') + cpu_read_register_16bit(&cpu->registers, 'de');
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (nnnn ^ cpu_read_register_16bit(&cpu->registers, 'hl') ^ cpu_read_register_16bit(&cpu->registers, 'de')) & 0x1000 ? 1 : 0;
                    cpu->registers.f.carry = (nnnn & 0xFFFF0000) ? 1 : 0;
                    cpu->registers.h = (nnnn & 0x0000FF00) >> 8;
                    cpu->registers.l = (nnnn & 0x000000FF); 
                    break;
                // ld a, [de]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'de'));
                    break;
                // dec de
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, 'de');
                    break;
                // inc e
                case 0xC:
                    cpu->registers.e++;
                    cpu->registers.f.zero = (cpu->registers.e == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.e & 0x0F) == 0x00);
                    break;
                // dec e
                case 0xD:
                    cpu->registers.e--;
                    cpu->registers.f.zero = (cpu->registers.e == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.e & 0x0F) == 0x0F); 
                    break;
                // ld e, n8
                case 0xE:
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.pc++); 
                    break;
                // rra
                case 0xF:
                    cpu->registers.f.carry = cpu->registers.a & 0x01;
                    cpu->registers.a = (cpu->registers.a >> 1) | (cpu->registers.a << 7);
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0; 
                    break;
            }
        break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x20:
            switch (opcode & 0x0F) {
                // jr nz, e8
                case 0x0:  
                    // jump conditional
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (!cpu->registers.f.zero) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // ld hl
                case 0x1:
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [hl+], a
                case 0x2:
                    // load from indirect hl, increment 
                    bus_write8(&cpu->bus, &cpu->registers.hl, cpu->registers.a);
                    cpu_increment_register_16bit(&cpu->registers, 'hl'); 
                    break;
                // inc hl
                case 0x3:
                    // increment the register de
                    cpu_increment_register_16bit(&cpu->registers, 'hl');                   
                    break;
                // inc h
                case 0x4:
                    cpu->registers.h++;
                    cpu->registers.f.zero = (cpu->registers.h == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.h & 0x0F) == 0x00); 
                    break;
                // dec h
                case 0x5:
                    cpu->registers.h--;
                    cpu->registers.f.zero = (cpu->registers.h == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.h & 0x0F) == 0x0F); 
                    break;
                // ld h, n8
                case 0x6:
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.pc++);  
                    break;
                // daa
                case 0x7:
                    uint8_t d = cpu->registers.a >> 4;
                    uint8_t d2 = cpu->registers.a & 0x0F;
                    if (cpu->registers.f.subtract) {
                        if (cpu->registers.f.half_carry) {
                            d2 -= 6;
                        }
                        if (cpu->registers.f.carry) {
                            d -= 6;
                        }
                        if (d2 > 9) {
                            d2 -= 6;
                        }
                        if (d > 9) {
                            d -= 6;
                            cpu->registers.f.carry = 1;
                        }
                    }
                    else {
                        if (cpu->registers.f.half_carry) {
                            d2 += 6;
                        }
                        if (cpu->registers.f.carry) {
                            d += 6;
                        }
                        if (d2 > 9) {
                            d2 -= 10;
                            d++;
                        }
                        if (d > 9) {
                            d -= 10;
                            cpu->registers.f.carry = 1;
                        }
                    }
                    
                    cpu->registers.a = ((d << 4) & 0xF0) | (d2 & 0x0F);
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.half_carry = 0;
                    break;
                // jr z, e8
                case 0x8:
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (cpu->registers.f.zero) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // add hl, hl
                case 0x9:
                    uint32_t nnnn = cpu_read_register_16bit(&cpu->registers, 'hl') + cpu_read_register_16bit(&cpu->registers, 'hl');
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (nnnn ^ cpu_read_register_16bit(&cpu->registers, 'hl') ^ cpu_read_register_16bit(&cpu->registers, 'hl')) & 0x1000 ? 1 : 0;
                    cpu->registers.f.carry = (nnnn & 0xFFFF0000) ? 1 : 0;
                    cpu->registers.h = (nnnn & 0x0000FF00) >> 8;
                    cpu->registers.l = (nnnn & 0x000000FF); 
                    break;
                // ld a, [hl+]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    cpu_increment_register_16bit(&cpu->registers, 'hl');
                    break;
                // dec hl
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, 'hl');
                    break;
                // inc l
                case 0xC:
                    cpu->registers.l++;
                    cpu->registers.f.zero = (cpu->registers.l == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.l & 0x0F) == 0x00);
                    break;
                // dec l
                case 0xD:
                    cpu->registers.l--;
                    cpu->registers.f.zero = (cpu->registers.l == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.l & 0x0F) == 0x0F); 
                    break;
                // ld l, n8
                case 0xE:
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.pc++); 
                    break;
                // cpl
                case 0xF:
                    cpu->registers.a = cpu->registers.a ^ 0xFF;
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = 1; 
                    break;
            }
            break;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0x30:
            switch (opcode & 0x0F) {
                // jr nc, e8
                case 0x0:  
                    // jump conditional
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (!cpu->registers.f.carry) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // ld sp, n16
                case 0x1:
                    cpu->registers.sp = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.sp |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    break;
                // ld [hl-], a
                case 0x2:
                    // load from indirect hl, decrement 
                    bus_write8(&cpu->bus, &cpu->registers.hl, cpu->registers.a);
                    cpu_decrement_register_16bit(&cpu->registers, 'hl'); 
                    break;
                // inc sp
                case 0x3:
                    // increment the register sp
                    cpu_increment_register_16bit(&cpu->registers, 'sp');                   
                    break;
                // inc hl
                case 0x4:
                    cpu_increment_register_16bit(&cpu->registers, 'hl');
                    cpu->registers.f.zero = (cpu_read_register_16bit(&cpu->registers, 'hl') == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, 'hl') & 0x0F) == 0x00); 
                    break;
                // dec hl
                case 0x5:
                    cpu_decrement_register_16bit(&cpu->registers, 'hl');
                    cpu->registers.f.zero = (cpu_read_register_16bit(&cpu->registers, 'hl') == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, 'hl') & 0x0F) == 0x0F); 
                    break;
                // ld [hl], n8
                case 0x6:
                    bus_write8(&cpu->bus, &cpu->registers.hl, bus_read8(&cpu->bus, cpu->registers.pc++)); 
                    break;
                // scf
                case 0x7:
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 1;
                    break;
                // jr c, e8
                case 0x8:
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (cpu->registers.f.carry) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // add hl, sp
                case 0x9:
                    uint32_t nnnn = cpu_read_register_16bit(&cpu->registers, 'hl') + cpu->registers.sp;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (nnnn ^ cpu_read_register_16bit(&cpu->registers, 'hl') ^ cpu->registers.sp) & 0x1000 ? 1 : 0;
                    cpu->registers.f.carry = (nnnn & 0xFFFF0000) ? 1 : 0;
                    cpu->registers.h = (nnnn & 0x0000FF00) >> 8;
                    cpu->registers.l = (nnnn & 0x000000FF); 
                    break;
                // ld a, [hl-]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    cpu_decrement_register_16bit(&cpu->registers, 'hl');
                    break;
                // dec sp
                case 0xB:
                    cpu->registers.sp--;
                    break;
                // inc a
                case 0xC:
                    cpu->registers.a++;
                    cpu->registers.f.zero = (cpu->registers.a == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0x0F) == 0x00);
                    break;
                // dec a
                case 0xD:
                    cpu->registers.a--;
                    cpu->registers.f.zero = (cpu->registers.a == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0x0F) == 0x0F); 
                    break;
                // ld a, n8
                case 0xE:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu->registers.pc++); 
                    break;
                // ccf
                case 0xF:
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = cpu->registers.f.carry ^ 0x1; 
                    break;
            }
        break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x40:
            switch (opcode & 0x0F) {
                // ld b, b
                case 0x0:  
                    break;
                // ld b, c
                case 0x1:  
                    cpu->registers.b = cpu->registers.c;
                    break;
                // ld b, d
                case 0x2:
                    cpu->registers.b = cpu->registers.d;
                    break;
                // ld b, e
                case 0x3:
                    cpu->registers.b = cpu->registers.e;
                    break;
                // ld b, h
                case 0x4:
                    cpu->registers.b = cpu->registers.h;  
                    break;
                // ld b, l
                case 0x5: 
                    cpu->registers.b = cpu->registers.l; 
                    break;
                // ld b, [hl]
                case 0x6:  
                    cpu->registers.b = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    break;
                // ld b, a
                case 0x7:
                    cpu->registers.b = cpu->registers.a;
                    break;
                // ld c, b
                case 0x8: 
                    cpu->registers.c = cpu->registers.b; 
                    break;
                // ld c, c
                case 0x9:  
                    break;
                // ld c, d
                case 0xA:
                    cpu->registers.c = cpu->registers.d;  
                    break;
                // ld c, e
                case 0xB:
                    cpu->registers.c = cpu->registers.e;  
                    break;
                // ld c, h
                case 0xC: 
                    cpu->registers.c = cpu->registers.h;
                    break;
                // ld c, l
                case 0xD: 
                    cpu->registers.c = cpu->registers.l; 
                    break;
                // ld c, [hl]
                case 0xE:
                    cpu->registers.c = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));  
                    break;
                // ld c, a
                case 0xF:
                     cpu->registers.c = cpu->registers.a;
                    break;
            }
            break;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        case 0x50:
            switch (opcode & 0x0F) {
                // ld d, b
                case 0x0:  
                    cpu->registers.d = cpu->registers.b;
                    break;
                // ld d, c
                case 0x1:  
                    cpu->registers.d = cpu->registers.c;
                    break;
                // ld d, d
                case 0x2:
                    break;
                // ld d, e
                case 0x3:
                    cpu->registers.d = cpu->registers.e;
                    break;
                // ld d, h
                case 0x4:
                    cpu->registers.d = cpu->registers.h;  
                    break;
                // ld d, l
                case 0x5: 
                    cpu->registers.d = cpu->registers.l; 
                    break;
                // ld d, [hl]
                case 0x6:  
                    cpu->registers.d = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    break;
                // ld d, a
                case 0x7:
                    cpu->registers.d = cpu->registers.a;
                    break;
                // ld e, b
                case 0x8: 
                    cpu->registers.e = cpu->registers.b; 
                    break;
                // ld e, c
                case 0x9: 
                    cpu->registers.e = cpu->registers.c; 
                    break;
                // ld e, d
                case 0xA:
                    cpu->registers.e = cpu->registers.d;  
                    break;
                // ld e, e
                case 0xB:
                 
                    break;
                // ld e, h
                case 0xC: 
                    cpu->registers.e = cpu->registers.h;
                    break;
                // ld e, l
                case 0xD: 
                    cpu->registers.e = cpu->registers.l; 
                    break;
                // ld e, [hl]
                case 0xE:
                    cpu->registers.e = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));  
                    break;
                // ld e, a
                case 0xF:
                     cpu->registers.e = cpu->registers.a;
                    break;
            }
        break;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x60:
            switch (opcode & 0x0F) {
                // ld h, b
                case 0x0:  
                    cpu->registers.h = cpu->registers.b;
                    break;
                // ld h, c
                case 0x1:  
                    cpu->registers.h = cpu->registers.c;
                    break;
                // ld h, d
                case 0x2:
                    cpu->registers.h = cpu->registers.d;
                    break;
                // ld h, e
                case 0x3:
                    cpu->registers.e = cpu->registers.e;
                    break;
                // ld h, h
                case 0x4:
                    break;
                // ld h, l
                case 0x5: 
                    cpu->registers.h = cpu->registers.l; 
                    break;
                // ld h, [hl]
                case 0x6:  
                    cpu->registers.h = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    break;
                // ld h, a
                case 0x7:
                    cpu->registers.h = cpu->registers.a;
                    break;
                // ld l, b
                case 0x8: 
                    cpu->registers.l = cpu->registers.b; 
                    break;
                // ld l, c
                case 0x9: 
                    cpu->registers.l = cpu->registers.c; 
                    break;
                // ld l, d
                case 0xA:
                    cpu->registers.l = cpu->registers.d;  
                    break;
                // ld l, e
                case 0xB:
                    cpu->registers.l = cpu->registers.e;
                    break;
                // ld l, h
                case 0xC: 
                    cpu->registers.l = cpu->registers.h;
                    break;
                // ld l, l
                case 0xD: 
                    break;
                // ld l, [hl]
                case 0xE:
                    cpu->registers.l = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));  
                    break;
                // ld l, a
                case 0xF:
                     cpu->registers.l = cpu->registers.a;
                    break;
            }
            break;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0x70:
            switch (opcode & 0x0F) {
                // ld [hl], b
                case 0x0:  
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.b);
                    break;
                // ld [hl], c
                case 0x1:  
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.c);
                    break;
                // ld [hl], d
                case 0x2:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.d);
                    break;
                // ld [hl], e
                case 0x3:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.e);
                    break;
                // ld [hl], h
                case 0x4:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.h);
                    break;
                // ld [hl], l
                case 0x5: 
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.l);
                    break;
                // halt
                case 0x6:  
                    // halt
                    break;
                // ld h, a
                case 0x7:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'), cpu->registers.a);
                    break;
                // ld a, b
                case 0x8: 
                    cpu->registers.a = cpu->registers.b; 
                    break;
                // ld a, c
                case 0x9: 
                    cpu->registers.a = cpu->registers.c;
                    break;
                // ld a, d
                case 0xA:
                    cpu->registers.a = cpu->registers.d;  
                    break;
                // ld a, e
                case 0xB:
                    cpu->registers.a = cpu->registers.e;
                    break;
                // ld a, h
                case 0xC: 
                    cpu->registers.a = cpu->registers.h;
                    break;
                // ld a, l
                case 0xD:
                    cpu->registers.a = cpu->registers.l;
                    break;
                // ld a, [hl]
                case 0xE:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));  
                    break;
                // ld a, a
                case 0xF:
                    break;
            }
        break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x80:
            switch (opcode & 0x0F) {
                // add a, b
                case 0x0:
                    uint16_t nn = cpu->registers.a + cpu->registers.b; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.b ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, c
                case 0x1:  
                    uint16_t nn = cpu->registers.a + cpu->registers.c; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.c ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, d
                case 0x2:  
                    uint16_t nn = cpu->registers.a + cpu->registers.d; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.d ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, e
                case 0x3:  
                    uint16_t nn = cpu->registers.a + cpu->registers.e; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.e ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, h
                case 0x4:
                    uint16_t nn = cpu->registers.a + cpu->registers.h; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.h ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // add a, l
                case 0x5:
                    uint16_t nn = cpu->registers.a + cpu->registers.l; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.l ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, [hl]
                case 0x6:
                    uint8_t n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    uint16_t nn = cpu->registers.a + n; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // add a, a
                case 0x7:
                    uint16_t nn = cpu->registers.a + cpu->registers.a; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = nn & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // adc a, b 
                case 0x8:
                    uint8_t n = cpu->registers.b;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // adc a, c 
                case 0x9:
                    uint8_t n = cpu->registers.c;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // adc a, d
                case 0xA: 
                    uint8_t n = cpu->registers.d;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // adc a, e
                case 0xB:  
                    uint8_t n = cpu->registers.e;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // adc a, h
                case 0xC:
                    uint8_t n = cpu->registers.h;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // adc a, l
                case 0xD: 
                    uint8_t n = cpu->registers.l;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // adc a, [hl]
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;
                    break;
                // adc a, a
                case 0xF:
                    uint8_t n = cpu->registers.a;
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
            }
            break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0x90:
            switch (opcode & 0x0F) {
                // sub a, b
                case 0x0:  
                    uint16_t nn = cpu->registers.a - cpu->registers.b; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.b ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // sub a, c
                case 0x1: 
                    uint16_t nn = cpu->registers.a - cpu->registers.c; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.c ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sub a, d
                case 0x2: 
                    uint16_t nn = cpu->registers.a - cpu->registers.d; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.d ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sub a, e
                case 0x3:  
                    uint16_t nn = cpu->registers.a - cpu->registers.e; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.e ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // sub a, h
                case 0x4:  
                    uint16_t nn = cpu->registers.a - cpu->registers.h; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.h ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // sub a, l
                case 0x5:  
                    uint16_t nn = cpu->registers.a - cpu->registers.l; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.l ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // sub a, [hl]
                case 0x6:  
                    uint8_t n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    uint16_t nn = cpu->registers.a - n;
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF; 
                    break;
                // sub a, a
                case 0x7: 
                    cpu->registers.a = 0x00;
                    cpu->registers.f.zero = 1;
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // sbc a, b
                case 0x8:
                    uint8_t n = cpu->registers.b;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, c
                case 0x9:  
                    uint8_t n = cpu->registers.c;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, d
                case 0xA:  
                    uint8_t n = cpu->registers.d;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, e
                case 0xB:  
                    uint8_t n = cpu->registers.e;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, h
                case 0xC:  
                    uint8_t n = cpu->registers.h;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, l
                case 0xD:  
                    uint8_t n = cpu->registers.l;
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, [hl]
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    cpu->registers.a = nn & 0xFF;  
                    break;
                // sbc a, a 
                case 0xF:  
                    cpu->registers.a = cpu->registers.f.carry ? 0xFF : 0x00;
                    cpu->registers.f.zero = cpu->registers.f.carry ? 0x00 : 0x01;
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = cpu->registers.f.carry;
                    break;
            }
        break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xA0:
            switch (opcode & 0x0F) {
                // and a, b
                case 0x0:  
                    cpu->registers.a = cpu->registers.a & cpu->registers.b;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // and a, c
                case 0x1:  
                    cpu->registers.a = cpu->registers.a & cpu->registers.c;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // and a, d
                case 0x2:  
                    cpu->registers.a = cpu->registers.a & cpu->registers.d;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // and a, e
                case 0x3:  
                    cpu->registers.a = cpu->registers.a & cpu->registers.e;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // and a, h
                case 0x4:
                    cpu->registers.a = cpu->registers.a & cpu->registers.h;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;  
                    break;
                // and a, l
                case 0x5:  
                    cpu->registers.a = cpu->registers.a & cpu->registers.l;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // and a, [hl]
                case 0x6:
                    cpu->registers.a = cpu->registers.a & bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;  
                    break;
                // and a, a 
                case 0x7:
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, b
                case 0x8:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.b;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, c
                case 0x9:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.c;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, d
                case 0xA:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.d;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, e
                case 0xB:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.e;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, h
                case 0xC:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.h;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, l
                case 0xD:  
                    cpu->registers.a = cpu->registers.a ^ cpu->registers.l;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // xor a, [hl]
                case 0xE: 
                    cpu->registers.a = cpu->registers.a ^ bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;  
                    break;
                // xor a
                case 0xF: 
                    cpu->registers.a = 0x00;
                    cpu->registers.f.zero = 1;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
            }
            break;


//////////////////////////////////////////////////////////////////////////////////////////////////
        
        case 0xB0:
            switch (opcode & 0x0F) {
                // or a, b
                case 0x0:  
                    cpu->registers.a = cpu->registers.a | cpu->registers.b;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, c
                case 0x1: 
                    cpu->registers.a = cpu->registers.a | cpu->registers.c;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;  
                    break;
                // or a, d
                case 0x2:  
                    cpu->registers.a = cpu->registers.a | cpu->registers.d;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, e
                case 0x3:  
                    cpu->registers.a = cpu->registers.a | cpu->registers.e;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, h
                case 0x4:  
                    cpu->registers.a = cpu->registers.a | cpu->registers.h;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, l
                case 0x5:  
                    cpu->registers.a = cpu->registers.a | cpu->registers.l;
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, [hl]
                case 0x6:  
                    cpu->registers.a = cpu->registers.a | bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // or a, a
                case 0x7:  
                    cpu->registers.f.zero = cpu->registers.a == 0x00;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
                // cp a, b
                case 0x8:    
                    uint16_t nn = cpu->registers.a - cpu->registers.b; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.b ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;  
                    break;
                // cp a, c
                case 0x9: 
                    uint16_t nn = cpu->registers.a - cpu->registers.c; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.c ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;   
                    break;
                // cp a, d
                case 0xA:  
                    uint16_t nn = cpu->registers.a - cpu->registers.d; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.d ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;  
                    break;
                // cp a, e
                case 0xB:  
                    uint16_t nn = cpu->registers.a - cpu->registers.e; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.e ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;  
                    break;
                // cp a, h
                case 0xC: 
                    uint16_t nn = cpu->registers.a - cpu->registers.h; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.h ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;   
                    break;
                // cp a, l
                case 0xD:  
                    uint16_t nn = cpu->registers.a - cpu->registers.l; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ cpu->registers.l ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;  
                    break;
                // cp a, [hl]
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, 'hl'));
                    uint16_t nn = cpu->registers.a - n; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;  
                    break;
                // cp a, a
                case 0xF:  
                    cpu->registers.f.zero = 1;
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0; 
                    break;
            }
        break;

///////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xC0:
            switch (opcode & 0x0F) {
                // ret nz
                case 0x0:  
                    if (!cpu->registers.f.zero) {
                        uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                        nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;
                        cpu->registers.pc = nn;
                    }
                    break;
                // pop bc
                case 0x1:  
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                // jp nz, a16
                case 0x2:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (!cpu->registers.f.zero) {
                        cpu->registers.pc = nn;
                    }
                    break;
                // jp imm
                case 0x3:
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    cpu->registers.pc = nn;
                    break;
                // call nz, imm
                case 0x4:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (!cpu->registers.f.zero) {
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
                        cpu->registers.pc = nn;
                    }
                    break;
                // push bc
                case 0x5: 
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.b);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.c);
                    break;
                // add a, n8
                case 0x6:  
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.a + n; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0; 
                    cpu->registers.a = nn & 0xFF;
                    break;
                // rst 0x0000
                case 0x7:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
                    cpu->registers.pc = 0x0000;
                    break;
                // ret z 
                case 0x8:  
                    if (cpu->registers.f.zero) {
                        uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                        nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;
                        cpu->registers.pc = nn;
                    }
                    break;
                // ret
                case 0x9:
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;  
                    cpu->registers.pc = nn;
                    break;
                // jp z, imm
                case 0xA:
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (cpu->registers.f.zero) {
                        cpu->registers.pc = nn;
                    }
                    break;
                
                // prefix
                case 0xB:  
                    // EXECUTE PREFIX
                    break;

                // call z, n8
                case 0xC:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (cpu->registers.f.zero) {
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                        cpu->registers.pc = nn;
                    }
                    break;
                // call n8
                case 0xD:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = nn;
                    break;
                // adc a, n8
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.a + n + cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0; 
                    cpu->registers.a = nn & 0xFF;
                    break;
                // rst 0x0008
                case 0xF:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0008;
                    break;
            }
            break;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        case 0xD0:
            switch (opcode & 0x0F) {
                // ret nc
                case 0x0:  
                    if (!cpu->registers.f.carry) {
                        uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                        nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;
                        cpu->registers.pc = nn;
                    }
                    break;
                // pop de
                case 0x1:  
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                // jp nc, a16
                case 0x2:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (!cpu->registers.f.carry) {
                        cpu->registers.pc = nn;
                    }
                    break;
                // na
                case 0x3:
                    break;
                // call nc, imm
                case 0x4:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (!cpu->registers.f.carry) {
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
                        cpu->registers.pc = nn;
                    }
                    break;
                // push de
                case 0x5: 
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.d);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.e);
                    break;
                // sub a, n8
                case 0x6:  
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.a - n; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0; 
                    cpu->registers.a = nn & 0xFF;
                    break;
                // rst 0x0010
                case 0x7:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF);
                    cpu->registers.pc = 0x0010;
                    break;
                // ret c
                case 0x8:  
                    if (cpu->registers.f.carry) {
                        uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                        nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;
                        cpu->registers.pc = nn;
                    }
                    break;


                // reti (NOT FINISHED)
                case 0x9:
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.sp++) << 8;  
                    cpu->registers.pc = nn;
                    // gb_ime = 1;
                    break;


                // jp c, imm
                case 0xA:
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (cpu->registers.f.carry) {
                        cpu->registers.pc = nn;
                    }
                    break;
                
                // na
                case 0xB: 
                    break;

                // call c, n8
                case 0xC:  
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    nn |= bus_read8(&cpu->bus, cpu->registers.pc++) << 8;
                    if (cpu->registers.f.carry) {
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                        bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                        cpu->registers.pc = nn;
                    }
                    break;
                // na
                case 0xD:  
                    break;
                // sbc a, n8
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.a - n - cpu->registers.f.carry; 
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0; 
                    cpu->registers.a = nn & 0xFF;
                    break;
                // rst 0x0018
                case 0xF:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0018;
                    break;
            }
        break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xE0:
            switch (opcode & 0x0F) {
                // ldh [a8], a
                case 0x0:  
                    bus_write8(&cpu->bus, 0xFF00 | bus_read8(&cpu->bus, cpu->registers.pc++), cpu->registers.a);
                    break;
                // pop hl
                case 0x1:
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                // ld [c], a
                case 0x2:  
                    bus_write8(&cpu->bus, 0xFF00 | cpu->registers.c, cpu->registers.a);
                    break;
                // na
                case 0x3:  
                    break;
                // na
                case 0x4:  
                    break;
                // push hl
                case 0x5:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.h);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.l); 
                    break;
                // and n8
                case 0x6: 
                    cpu->registers.a = cpu->registers.a & bus_read8(&cpu->bus, cpu->registers.pc++); 
                    cpu->registers.f.zero = (cpu->registers.a == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                // rst 0x0020
                case 0x7: 
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0020;
                    break;
                // add sp, n8
                case 0x8:  
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.sp + sn; 
                    if (sn >= 0) {
                        cpu->registers.f.zero = 0;
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.sp ^ sn ^ nn) & 0x1000) ? 1 : 0;
                        cpu->registers.f.carry = (cpu->registers.sp > nn);
                    }
                    else {
                        cpu->registers.f.zero = 0;
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.sp ^ sn ^ nn) & 0x1000) ? 1 : 0;
                        cpu->registers.f.carry = (cpu->registers.sp < nn);
                    }
                    cpu->registers.sp = nn;
                    break;
                // jp [hl]
                case 0x9:  
                    cpu->registers.pc = cpu->registers.hl;
                    break;
                // ld [n8], a
                case 0xA:  
                    bus_write8(&cpu->bus, bus_read8(&cpu->bus, cpu->registers.pc++) | bus_read8(&cpu->bus, cpu->registers.pc++) << 8, cpu->registers.a);
                    break;
                // na
                case 0xB:  
                    break;
                // na
                case 0xC:  
                    break;
                // na
                case 0xD:  
                    break;
                // xor n8
                case 0xE:  
                    cpu->registers.a = cpu->registers.a ^ bus_read8(&cpu->bus, cpu->registers.pc++); 
                    cpu->registers.f.zero = (cpu->registers.a == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // rst 0x0028
                case 0xF:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0028;
                    break;
            }
            break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0xF0:
            switch (opcode & 0x0F) {
                // ldh a, [a8]
                case 0x0:  
                    cpu->registers.a = bus_read8(&cpu->bus, (0xFF00 | bus_read8(&cpu->bus, cpu->registers.pc++)));
                    break;
                // pop af
                case 0x1:
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.f.zero = (n >> 7) & 1;
                    cpu->registers.f.subtract = (n >> 6) & 1;
                    cpu->registers.f.half_carry = (n >> 5) & 1;
                    cpu->registers.f.carry = (n >> 4) & 1;
                    cpu->registers.a = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                // ld a, [c]
                case 0x2:  
                    cpu->registers.a = bus_read8(&cpu->bus, (0xFF00 | cpu->registers.c));
                    break;


                // di
                case 0x3:  
                    // gb_ime = 0;
                    break;

                
                // na
                case 0x4:  
                    break;
                // push af
                case 0x5:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.a);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.f.zero << 7 | cpu->registers.f.subtract << 6 | cpu->registers.f.half_carry << 5 | cpu->registers.f.carry << 4); 
                    break;
                // or n8
                case 0x6: 
                    cpu->registers.a = cpu->registers.a | bus_read8(&cpu->bus, cpu->registers.pc++); 
                    cpu->registers.f.zero = (cpu->registers.a == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                // rst 0x0030
                case 0x7: 
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0030;
                    break;
                // ld hl, sp +- n8
                case 0x8:  
                    signed char sn = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.sp + sn; 
                    if (sn >= 0) {
                        cpu->registers.f.zero = 0;
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.sp ^ sn ^ nn) & 0x1000) ? 1 : 0;
                        cpu->registers.f.carry = (cpu->registers.sp > nn);
                    }
                    else {
                        cpu->registers.f.zero = 0;
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.sp ^ sn ^ nn) & 0x1000) ? 1 : 0;
                        cpu->registers.f.carry = (cpu->registers.sp < nn);
                    }
                    cpu->registers.h = (nn & 0xFF00) >> 8;
                    cpu->registers.l = (nn & 0x00FF);
                    break;
                // ld sp, hl
                case 0x9:  
                    cpu->registers.sp = cpu->registers.hl;
                    break;
                // ld a, [n8]
                case 0xA:  
                    cpu->registers.a = bus_read8(&cpu->bus, (bus_read8(&cpu->bus, cpu->registers.pc++) | bus_read8(&cpu->bus, cpu->registers.pc++) << 8));
                    break;


                // ei 
                case 0xB:  
                    // gb_ime = 1;
                    break;


                // na
                case 0xC:  
                    break;
                // na
                case 0xD:  
                    break;
                // xor n8
                case 0xE:  
                    uint8_t n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    uint16_t nn = cpu->registers.a - n;
                    cpu->registers.f.zero = ((nn & 0xFF) == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = (cpu->registers.a ^ n ^ nn) & 0x10 ? 1 : 0;
                    cpu->registers.f.carry = (nn & 0xFF00) ? 1 : 0;
                    break;
                // rst 0x0038
                case 0xF:  
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc >> 8);
                    bus_write8(&cpu->bus, --cpu->registers.sp, cpu->registers.pc & 0xFF); 
                    cpu->registers.pc = 0x0038;
                    break;
            }
        break;

        

        
        default:
            
            break;
    }
}