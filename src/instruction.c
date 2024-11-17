#include "../include/instruction.h"
#include "../include/cpu.h"

void instruction_execute(cpu *cpu, uint8_t opcode) {

    uint16_t nn;
    uint8_t n;
    int16_t sn;
    uint32_t result;

    uint8_t cb_op_tcycles[0x100] = {
        //   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x00
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x10
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x20
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x30
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,    // 0x40
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,    // 0x50
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,    // 0x60
        8, 8, 8, 8, 8, 8,12, 8, 8, 8, 8, 8, 8, 8,12, 8,    // 0x70
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x80
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0x90
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0xA0
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0xB0
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0xC0
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0xD0
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8,    // 0xE0
        8, 8, 8, 8, 8, 8,16, 8, 8, 8, 8, 8, 8, 8,16, 8     // 0xF0
    };
    

    // ("Executing opcode: 0x%02X\n", opcode);

    switch (opcode & 0xF0) {
        case 0x00:
            switch (opcode & 0x0F) {
                // nop
                case 0x0:  
                    // do nothing
                    break;
                // ld bc 
                case 0x1:
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [bc], a
                case 0x2: 
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "bc"), cpu->registers.a);
                    break;
                // inc bc
                case 0x3:  
                    cpu_increment_register_16bit(&cpu->registers, "bc");
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
                case 0x8:  
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    bus_write16(&cpu->bus, nn, cpu->registers.sp);
                    break;
                // add hl, bc
                case 0x9:
                    result = cpu_read_register_16bit(&cpu->registers, "hl") + cpu_read_register_16bit(&cpu->registers, "bc");
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, "hl") & 0xFFF) + (cpu_read_register_16bit(&cpu->registers, "bc") & 0xFFF) > 0xFFF);
                    cpu->registers.f.carry = (result & 0xFFFF0000) ? 1 : 0;
                    cpu_write_register_16bit(&cpu->registers, "hl", result & 0xFFFF);
                    break;
                // ld a, [bc]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "bc"));
                    break;
                // dec bc
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, "bc");
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
                // stop
                case 0x0:  
                    // Implement STOP instruction
                    break;
                // ld de, n16
                case 0x1:
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [de], a
                case 0x2:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "de"), cpu->registers.a);
                    break;
                // inc de
                case 0x3:
                    cpu_increment_register_16bit(&cpu->registers, "de");                   
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
                    n = (cpu->registers.a & 0x80) >> 7;
                    cpu->registers.a = (cpu->registers.a << 1) | cpu->registers.f.carry;
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = n;
                    break;
                // jr e8
                case 0x8:
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.pc += sn;
                    break;
                // add hl, de
                case 0x9:
                    result = cpu_read_register_16bit(&cpu->registers, "hl") + cpu_read_register_16bit(&cpu->registers, "de");
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, "hl") & 0xFFF) + (cpu_read_register_16bit(&cpu->registers, "de") & 0xFFF) > 0xFFF);
                    cpu->registers.f.carry = (result & 0xFFFF0000) ? 1 : 0;
                    cpu_write_register_16bit(&cpu->registers, "hl", result & 0xFFFF);
                    break;
                // ld a, [de]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "de"));
                    break;
                // dec de
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, "de");
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
                    n = cpu->registers.a & 0x01;
                    cpu->registers.a = (cpu->registers.a >> 1) | (cpu->registers.f.carry << 7);
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = n;
                    break;
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x20:
            switch (opcode & 0x0F) {
                // jr nz, e8
                case 0x0:  
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (!cpu->registers.f.zero) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // ld hl, n16
                case 0x1:
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.pc++);
                    break;
                // ld [hl+], a
                case 0x2:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), cpu->registers.a);
                    cpu_increment_register_16bit(&cpu->registers, "hl");
                    break;
                // inc hl
                case 0x3:
                    cpu_increment_register_16bit(&cpu->registers, "hl");
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
                    if (!cpu->registers.f.subtract) {
                        if (cpu->registers.f.carry || cpu->registers.a > 0x99) {
                            cpu->registers.a += 0x60;
                            cpu->registers.f.carry = 1;
                        }
                        if (cpu->registers.f.half_carry || (cpu->registers.a & 0x0F) > 0x09) {
                            cpu->registers.a += 0x06;
                        }
                    } else if (cpu->registers.f.carry) {
                        cpu->registers.a -= 0x60;
                        if (cpu->registers.f.half_carry) {
                            cpu->registers.a -= 0x06;
                        }
                    } else if (cpu->registers.f.half_carry) {
                        cpu->registers.a -= 0x06;
                    }
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.half_carry = 0;
                    break;
                // jr z, e8
                case 0x8:
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (cpu->registers.f.zero) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // add hl, hl
                case 0x9:
                    result = cpu_read_register_16bit(&cpu->registers, "hl") * 2;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, "hl") & 0xFFF) * 2 > 0xFFF);
                    cpu->registers.f.carry = (result > 0xFFFF);
                    cpu_write_register_16bit(&cpu->registers, "hl", result & 0xFFFF);
                    break;
                // ld a, [hl+]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    cpu_increment_register_16bit(&cpu->registers, "hl");
                    break;
                // dec hl
                case 0xB:
                    cpu_decrement_register_16bit(&cpu->registers, "hl");
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
                    cpu->registers.a = ~cpu->registers.a;
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
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
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
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), cpu->registers.a);
                    cpu_decrement_register_16bit(&cpu->registers, "hl");
                    break;
                // inc sp
                case 0x3:
                    cpu->registers.sp++;
                    break;
                // inc [hl]
                case 0x4:
                    n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    n++;
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), n);
                    cpu->registers.f.zero = (n == 0x00);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((n & 0x0F) == 0x00);
                    break;
                // dec [hl]
                case 0x5:
                    n = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    n--;
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), n);
                    cpu->registers.f.zero = (n == 0x00);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((n & 0x0F) == 0x0F);
                    break;
                // ld [hl], n8
                case 0x6:
                    bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), bus_read8(&cpu->bus, cpu->registers.pc++));
                    break;
                // scf
                case 0x7:
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 1;
                    break;
                // jr c, e8
                case 0x8:
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    if (cpu->registers.f.carry) {
                        cpu->registers.pc += sn;
                    }
                    break;
                // add hl, sp
                case 0x9:
                    result = cpu_read_register_16bit(&cpu->registers, "hl") + cpu->registers.sp;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu_read_register_16bit(&cpu->registers, "hl") & 0xFFF) + (cpu->registers.sp & 0xFFF) > 0xFFF);
                    cpu->registers.f.carry = (result > 0xFFFF);
                    cpu_write_register_16bit(&cpu->registers, "hl", result & 0xFFFF);
                    break;
                // ld a, [hl-]
                case 0xA:
                    cpu->registers.a = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    cpu_decrement_register_16bit(&cpu->registers, "hl");
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
                    cpu->registers.f.carry = !cpu->registers.f.carry;
                    break;
            }
            break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0x40:
        case 0x50:
        case 0x60:
        case 0x70:
            {
                // halt goes first
                if (opcode == 0x76) {
                    cpu->halted = 1;
                    // cpu->registers.pc++;
                    // printf("halted called in instruction");
                    break;
                }

                uint8_t src = opcode & 0x07;
                uint8_t dst = (opcode >> 3) & 0x07;
                uint8_t value;

                // source selection
                switch (src) {
                    case 0: value = cpu->registers.b; break;
                    case 1: value = cpu->registers.c; break;
                    case 2: value = cpu->registers.d; break;
                    case 3: value = cpu->registers.e; break;
                    case 4: value = cpu->registers.h; break;
                    case 5: value = cpu->registers.l; break;
                    case 6: value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl")); break;
                    case 7: value = cpu->registers.a; break;
                }

                // destination selection
                switch (dst) {
                    case 0: cpu->registers.b = value; break;
                    case 1: cpu->registers.c = value; break;
                    case 2: cpu->registers.d = value; break;
                    case 3: cpu->registers.e = value; break;
                    case 4: cpu->registers.h = value; break;
                    case 5: cpu->registers.l = value; break;
                    case 6: bus_write8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"), value); break;
                    case 7: cpu->registers.a = value; break;
                }
            }
            break;

        case 0x80:
            switch (opcode & 0x0F) {
                // ADD A, r8
                case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x7:
                    {
                        uint8_t r = (opcode & 0x07);
                        uint8_t value = r == 0 ? cpu->registers.b :
                                        r == 1 ? cpu->registers.c :
                                        r == 2 ? cpu->registers.d :
                                        r == 3 ? cpu->registers.e :
                                        r == 4 ? cpu->registers.h :
                                        r == 5 ? cpu->registers.l :
                                                 cpu->registers.a;
                        uint16_t result = cpu->registers.a + value;
                        cpu->registers.f.zero = ((result & 0xFF) == 0);
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (value & 0xF) > 0xF);
                        cpu->registers.f.carry = (result > 0xFF);
                        cpu->registers.a = result & 0xFF;
                    }
                    break;
                // ADD A, [HL]
                case 0x6:
                    {
                        uint8_t value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                        uint16_t result = cpu->registers.a + value;
                        cpu->registers.f.zero = ((result & 0xFF) == 0);
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (value & 0xF) > 0xF);
                        cpu->registers.f.carry = (result > 0xFF);
                        cpu->registers.a = result & 0xFF;
                    }
                    break;
                // ADC A, r8
                case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xF:
                    {
                        uint8_t r = (opcode & 0x07);
                        uint8_t value = r == 0 ? cpu->registers.b :
                                        r == 1 ? cpu->registers.c :
                                        r == 2 ? cpu->registers.d :
                                        r == 3 ? cpu->registers.e :
                                        r == 4 ? cpu->registers.h :
                                        r == 5 ? cpu->registers.l :
                                                 cpu->registers.a;
                        uint16_t result = cpu->registers.a + value + cpu->registers.f.carry;
                        cpu->registers.f.zero = ((result & 0xFF) == 0);
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (value & 0xF) + cpu->registers.f.carry > 0xF);
                        cpu->registers.f.carry = (result > 0xFF);
                        cpu->registers.a = result & 0xFF;
                    }
                    break;
                // ADC A, [HL]
                case 0xE:
                    {
                        uint8_t value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                        uint16_t result = cpu->registers.a + value + cpu->registers.f.carry;
                        cpu->registers.f.zero = ((result & 0xFF) == 0);
                        cpu->registers.f.subtract = 0;
                        cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (value & 0xF) + cpu->registers.f.carry > 0xF);
                        cpu->registers.f.carry = (result > 0xFF);
                        cpu->registers.a = result & 0xFF;
                    }
                    break;
            }
            break;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0x90:
            switch (opcode & 0x0F) {
                // SUB A, r8
                case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x7: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    uint16_t result = cpu->registers.a - value;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (value & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // SUB A, [HL]
                case 0x6: {
                    uint8_t value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    uint16_t result = cpu->registers.a - value;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (value & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // SBC A, r8
                case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xF: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    uint16_t result = cpu->registers.a - value - cpu->registers.f.carry;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < ((value & 0xF) + cpu->registers.f.carry));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // SBC A, [HL]
                case 0xE: {
                    uint8_t value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    uint16_t result = cpu->registers.a - value - cpu->registers.f.carry;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < ((value & 0xF) + cpu->registers.f.carry));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
            }
            break;

        case 0xA0:
            switch (opcode & 0x0F) {
                // AND A, r8
                case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x7: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    cpu->registers.a &= value;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // AND A, [HL]
                case 0x6: {
                    cpu->registers.a &= bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // XOR A, r8
                case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xF: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    cpu->registers.a ^= value;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // XOR A, [HL]
                case 0xE: {
                    cpu->registers.a ^= bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
            }
            break;

        case 0xB0:
            switch (opcode & 0x0F) {
                // OR A, r8
                case 0x0: case 0x1: case 0x2: case 0x3: case 0x4: case 0x5: case 0x7: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    cpu->registers.a |= value;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // OR A, [HL]
                case 0x6: {
                    cpu->registers.a |= bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // CP A, r8
                case 0x8: case 0x9: case 0xA: case 0xB: case 0xC: case 0xD: case 0xF: {
                    uint8_t r = (opcode & 0x07);
                    uint8_t value = (r == 0) ? cpu->registers.b :
                                    (r == 1) ? cpu->registers.c :
                                    (r == 2) ? cpu->registers.d :
                                    (r == 3) ? cpu->registers.e :
                                    (r == 4) ? cpu->registers.h :
                                    (r == 5) ? cpu->registers.l : cpu->registers.a;
                    uint16_t result = cpu->registers.a - value;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (value & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    break;
                }
                // CP A, [HL]
                case 0xE: {
                    uint8_t value = bus_read8(&cpu->bus, cpu_read_register_16bit(&cpu->registers, "hl"));
                    uint16_t result = cpu->registers.a - value;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (value & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    break;
                }
            }
            break;

///////////////////////////////////////////////////////////////////////////////////////////////////////

        case 0xC0:
            switch (opcode & 0x0F) {
                // RET NZ
                case 0x0: {
                    if (!cpu->registers.f.zero) {
                        cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.sp);
                        cpu->registers.sp += 2;
                    }
                    break;
                }
                // POP BC
                case 0x1: {
                    cpu->registers.c = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.b = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                }
                // JP NZ, a16
                case 0x2: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (!cpu->registers.f.zero) {
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // JP a16
                case 0x3: {
                    cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.pc++);
                    break;
                }
                // CALL NZ, a16
                case 0x4: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (!cpu->registers.f.zero) {
                        cpu->registers.sp -= 2;
                        bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // PUSH BC
                case 0x5: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu_read_register_16bit(&cpu->registers, "bc"));
                    break;
                }
                // ADD A, d8
                case 0x6: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.a + n;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (n & 0xF) > 0xF);
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // RST 00H
                case 0x7: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0000;
                    break;
                }
                // RET Z
                case 0x8: {
                    if (cpu->registers.f.zero) {
                        cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.sp);
                        cpu->registers.sp += 2;
                    }
                    break;
                }
                // RET
                case 0x9: {
                    uint16_t nn = bus_read8(&cpu->bus, cpu->registers.sp++);
                    nn |= (uint16_t)bus_read8(&cpu->bus, cpu->registers.sp++) << 8;
                    cpu->registers.pc = nn;
                    break;
                }
                // JP Z, a16
                case 0xA: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (cpu->registers.f.zero) {
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // PREFIX CB
                case 0xB: {
                    // execute CB instructions
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    prefix_instruction_execute(cpu, n);
                    cpu->counter = cb_op_tcycles[n];
                    break;
                }
                // CALL Z, a16
                case 0xC: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (cpu->registers.f.zero) {
                        cpu->registers.sp -= 2;
                        bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // CALL a16
                case 0xD: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = nn;
                    break;
                }
                // ADC A, d8
                case 0xE: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.a + n + cpu->registers.f.carry;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) + (n & 0xF) + cpu->registers.f.carry > 0xF);
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // RST 08H
                case 0xF: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0008;
                    break;
                }
            }
            break;

        case 0xD0:
            switch (opcode & 0x0F) {
                // RET NC
                case 0x0: {
                    if (!cpu->registers.f.carry) {
                        cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.sp);
                        cpu->registers.sp += 2;
                    }
                    break;
                }
                // POP DE
                case 0x1: {
                    cpu->registers.e = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.d = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                }
                // JP NC, a16
                case 0x2: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (!cpu->registers.f.carry) {
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // CALL NC, a16
                case 0x4: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (!cpu->registers.f.carry) {
                        cpu->registers.sp -= 2;
                        bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // PUSH DE
                case 0x5: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu_read_register_16bit(&cpu->registers, "de"));
                    break;
                }
                // SUB d8
                case 0x6: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.a - n;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (n & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // RST 10H
                case 0x7: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0010;
                    break;
                }
                // RET C
                case 0x8: {
                    if (cpu->registers.f.carry) {
                        cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.sp);
                        cpu->registers.sp += 2;
                    }
                    break;
                }
                // RETI
                case 0x9: {
                    cpu->registers.pc = bus_read16(&cpu->bus, cpu->registers.sp);
                    cpu->registers.sp += 2;
                    cpu->ime = 1;  // enable interrupts
                    break;
                }
                // JP C, a16
                case 0xA: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (cpu->registers.f.carry) {
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // CALL C, a16
                case 0xC: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    if (cpu->registers.f.carry) {
                        cpu->registers.sp -= 2;
                        bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                        cpu->registers.pc = nn;
                    }
                    break;
                }
                // SBC A, d8
                case 0xE: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.a - n - cpu->registers.f.carry;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < ((n & 0xF) + cpu->registers.f.carry));
                    cpu->registers.f.carry = (result > 0xFF);
                    cpu->registers.a = result & 0xFF;
                    break;
                }
                // RST 18H
                case 0xF: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0018;
                    break;
                }
            }
            break;

        case 0xE0:
            switch (opcode & 0x0F) {
                // LDH (a8), A
                case 0x0: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    bus_write8(&cpu->bus, 0xFF00 + n, cpu->registers.a);
                    break;
                }
                // POP HL
                case 0x1: {
                    cpu->registers.l = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.h = bus_read8(&cpu->bus, cpu->registers.sp++);
                    break;
                }
                // LD (C), A
                case 0x2: {
                    bus_write8(&cpu->bus, 0xFF00 + cpu->registers.c, cpu->registers.a);
                    break;
                }
                // PUSH HL
                case 0x5: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu_read_register_16bit(&cpu->registers, "hl"));
                    break;
                }
                // AND d8
                case 0x6: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.a &= n;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 1;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // RST 20H
                case 0x7: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0020;
                    break;
                }
                // ADD SP, r8
                case 0x8: {
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.sp + sn;
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.sp & 0xF) + (sn & 0xF) > 0xF);
                    cpu->registers.f.carry = ((cpu->registers.sp & 0xFF) + (sn & 0xFF) > 0xFF);
                    cpu->registers.sp = result & 0xFFFF;
                    break;
                }
                // JP (HL)
                case 0x9: {
                    cpu->registers.pc = cpu_read_register_16bit(&cpu->registers, "hl");
                    break;
                }
                // LD (a16), A
                case 0xA: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    bus_write8(&cpu->bus, nn, cpu->registers.a);
                    break;
                }
                // XOR d8
                case 0xE: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.a ^= n;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // RST 28H
                case 0xF: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0028;
                    break;
                }
            }
            break;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        
        case 0xF0:
            switch (opcode & 0x0F) {
                // LDH A, (a8)
                case 0x0: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.a = bus_read8(&cpu->bus, 0xFF00 + n);
                    break;
                }
                // POP AF
                case 0x1: {
                    uint8_t flags = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.a = bus_read8(&cpu->bus, cpu->registers.sp++);
                    cpu->registers.f.zero = (flags >> 7) & 1;
                    cpu->registers.f.subtract = (flags >> 6) & 1;
                    cpu->registers.f.half_carry = (flags >> 5) & 1;
                    cpu->registers.f.carry = (flags >> 4) & 1;
                    break;
                }
                // LD A, (C)
                case 0x2: {
                    cpu->registers.a = bus_read8(&cpu->bus, 0xFF00 + cpu->registers.c);
                    break;
                }
                // DI
                case 0x3: {
                    cpu->ime = 0;
                    break;
                }
                // PUSH AF
                case 0x5: {
                    cpu->registers.sp -= 2;
                    uint8_t flags = (cpu->registers.f.zero << 7) |
                                    (cpu->registers.f.subtract << 6) |
                                    (cpu->registers.f.half_carry << 5) |
                                    (cpu->registers.f.carry << 4);
                    bus_write8(&cpu->bus, cpu->registers.sp, flags);
                    bus_write8(&cpu->bus, cpu->registers.sp + 1, cpu->registers.a);
                    break;
                }
                // OR d8
                case 0x6: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    cpu->registers.a |= n;
                    cpu->registers.f.zero = (cpu->registers.a == 0);
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = 0;
                    cpu->registers.f.carry = 0;
                    break;
                }
                // RST 30H
                case 0x7: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0030;
                    break;
                }
                // LD HL, SP+r8
                case 0x8: {
                    sn = (int8_t)bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.sp + sn;
                    cpu->registers.f.zero = 0;
                    cpu->registers.f.subtract = 0;
                    cpu->registers.f.half_carry = ((cpu->registers.sp & 0xF) + (sn & 0xF) > 0xF);
                    cpu->registers.f.carry = ((cpu->registers.sp & 0xFF) + (sn & 0xFF) > 0xFF);
                    cpu_write_register_16bit(&cpu->registers, "hl", result & 0xFFFF);
                    break;
                }
                // LD SP, HL
                case 0x9: {
                    cpu->registers.sp = cpu_read_register_16bit(&cpu->registers, "hl");
                    break;
                }
                // LD A, (a16)
                case 0xA: {
                    nn = bus_read16(&cpu->bus, cpu->registers.pc);
                    cpu->registers.pc += 2;
                    cpu->registers.a = bus_read8(&cpu->bus, nn);
                    break;
                }
                // EI
                case 0xB: {
                    cpu->ime = 1;
                    break;
                }
                // CP d8
                case 0xE: {
                    n = bus_read8(&cpu->bus, cpu->registers.pc++);
                    result = cpu->registers.a - n;
                    cpu->registers.f.zero = ((result & 0xFF) == 0);
                    cpu->registers.f.subtract = 1;
                    cpu->registers.f.half_carry = ((cpu->registers.a & 0xF) < (n & 0xF));
                    cpu->registers.f.carry = (result > 0xFF);
                    break;
                }
                // RST 38H
                case 0xF: {
                    cpu->registers.sp -= 2;
                    bus_write16(&cpu->bus, cpu->registers.sp, cpu->registers.pc);
                    cpu->registers.pc = 0x0038;
                    break;
                }
            }
            break;

        default:
            
            break;
    }
}