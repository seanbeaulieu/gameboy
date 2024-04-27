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
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.pc++);
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
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.pc++);
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