#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/jsontest.h"

typedef struct {
    const char* name;
    struct {
        uint16_t pc;
        uint16_t sp;
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t h;
        uint8_t l;
        struct {
            uint16_t address;
            uint8_t value;
        } *ram;
        int ram_size;
    } initial;
    
    struct {
        uint16_t pc;
        uint16_t sp;
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t f;
        uint8_t h;
        uint8_t l;
        struct {
            uint16_t address;
            uint8_t value;
        } *ram;
        int ram_size;
    } final;
} test_case;

// load and parse a single test from json
test_case* parse_test(cJSON* json) {
    test_case* test = malloc(sizeof(test_case));
    memset(test, 0, sizeof(test_case));
    
    // get test name
    cJSON* name = cJSON_GetObjectItem(json, "name");
    test->name = strdup(name->valuestring);
    
    // parse initial state
    cJSON* initial = cJSON_GetObjectItem(json, "initial");
    test->initial.pc = cJSON_GetObjectItem(initial, "pc")->valueint;
    test->initial.sp = cJSON_GetObjectItem(initial, "sp")->valueint;
    test->initial.a = cJSON_GetObjectItem(initial, "a")->valueint;
    test->initial.b = cJSON_GetObjectItem(initial, "b")->valueint;
    test->initial.c = cJSON_GetObjectItem(initial, "c")->valueint;
    test->initial.d = cJSON_GetObjectItem(initial, "d")->valueint;
    test->initial.e = cJSON_GetObjectItem(initial, "e")->valueint;
    test->initial.f = cJSON_GetObjectItem(initial, "f")->valueint;
    test->initial.h = cJSON_GetObjectItem(initial, "h")->valueint;
    test->initial.l = cJSON_GetObjectItem(initial, "l")->valueint;
    
    // parse initial ram
    cJSON* ram = cJSON_GetObjectItem(initial, "ram");
    test->initial.ram_size = cJSON_GetArraySize(ram);
    test->initial.ram = malloc(sizeof(test->initial.ram[0]) * test->initial.ram_size);
    
    for (int i = 0; i < test->initial.ram_size; i++) {
        cJSON* ram_entry = cJSON_GetArrayItem(ram, i);
        test->initial.ram[i].address = cJSON_GetArrayItem(ram_entry, 0)->valueint;
        test->initial.ram[i].value = cJSON_GetArrayItem(ram_entry, 1)->valueint;
    }
    
    // parse final state
    cJSON* final = cJSON_GetObjectItem(json, "final");
    test->final.pc = cJSON_GetObjectItem(final, "pc")->valueint;
    test->final.sp = cJSON_GetObjectItem(final, "sp")->valueint;
    test->final.a = cJSON_GetObjectItem(final, "a")->valueint;
    test->final.b = cJSON_GetObjectItem(final, "b")->valueint;
    test->final.c = cJSON_GetObjectItem(final, "c")->valueint;
    test->final.d = cJSON_GetObjectItem(final, "d")->valueint;
    test->final.e = cJSON_GetObjectItem(final, "e")->valueint;
    test->final.f = cJSON_GetObjectItem(final, "f")->valueint;
    test->final.h = cJSON_GetObjectItem(final, "h")->valueint;
    test->final.l = cJSON_GetObjectItem(final, "l")->valueint;
    
    // parse final ram
    ram = cJSON_GetObjectItem(final, "ram");
    test->final.ram_size = cJSON_GetArraySize(ram);
    test->final.ram = malloc(sizeof(test->final.ram[0]) * test->final.ram_size);
    
    for (int i = 0; i < test->final.ram_size; i++) {
        cJSON* ram_entry = cJSON_GetArrayItem(ram, i);
        test->final.ram[i].address = cJSON_GetArrayItem(ram_entry, 0)->valueint;
        test->final.ram[i].value = cJSON_GetArrayItem(ram_entry, 1)->valueint;
    }
    
    return test;
}

// set up initial cpu state from test case
void setup_cpu_state(cpu *cpu, test_case *test) {
    // set registers
    cpu->registers.pc = test->initial.pc;
    cpu->registers.sp = test->initial.sp;
    cpu->registers.a = test->initial.a;
    cpu->registers.b = test->initial.b;
    cpu->registers.c = test->initial.c;
    cpu->registers.d = test->initial.d;
    cpu->registers.e = test->initial.e;
    cpu->registers.f = byte_to_flags_register(test->initial.f);
    cpu->registers.h = test->initial.h;
    cpu->registers.l = test->initial.l;
    
    // clear memory first
    memset(cpu->bus.memory, 0, 65536);
    
    // set memory values
    for (int i = 0; i < test->initial.ram_size; i++) {
        bus_write8(&cpu->bus, test->initial.ram[i].address, test->initial.ram[i].value);
    }
}

// verify cpu state matches expected final state
bool verify_cpu_state(cpu *cpu, test_case *test) {
    bool passed = true;
    
    // check registers
    if (cpu->registers.pc != test->final.pc) {
        printf("pc mismatch: expected 0x%04X, got 0x%04X\n", test->final.pc, cpu->registers.pc);
        passed = false;
    }
    if (cpu->registers.sp != test->final.sp) {
        printf("sp mismatch: expected 0x%04X, got 0x%04X\n", test->final.sp, cpu->registers.sp);
        passed = false;
    }
    if (cpu->registers.a != test->final.a) {
        printf("a mismatch: expected 0x%02X, got 0x%02X\n", test->final.a, cpu->registers.a);
        passed = false;
    }
    if (cpu->registers.b != test->final.b) {
        printf("b mismatch: expected 0x%02X, got 0x%02X\n", test->final.b, cpu->registers.b);
        passed = false;
    }
    if (cpu->registers.c != test->final.c) {
        printf("c mismatch: expected 0x%02X, got 0x%02X\n", test->final.c, cpu->registers.c);
        passed = false;
    }
    if (cpu->registers.d != test->final.d) {
        printf("d mismatch: expected 0x%02X, got 0x%02X\n", test->final.d, cpu->registers.d);
        passed = false;
    }
    if (cpu->registers.e != test->final.e) {
        printf("e mismatch: expected 0x%02X, got 0x%02X\n", test->final.e, cpu->registers.e);
        passed = false;
    }
    
    uint8_t actual_f = flags_register_to_byte(cpu->registers.f);
    if (actual_f != test->final.f) {
        printf("f mismatch: expected 0x%02X, got 0x%02X\n", test->final.f, actual_f);
        passed = false;
    }
    
    if (cpu->registers.h != test->final.h) {
        printf("h mismatch: expected 0x%02X, got 0x%02X\n", test->final.h, cpu->registers.h);
        passed = false;
    }
    if (cpu->registers.l != test->final.l) {
        printf("l mismatch: expected 0x%02X, got 0x%02X\n", test->final.l, cpu->registers.l);
        passed = false;
    }
    
    // check memory
    for (int i = 0; i < test->final.ram_size; i++) {
        uint8_t actual = bus_read8(&cpu->bus, test->final.ram[i].address);
        if (actual != test->final.ram[i].value) {
            printf("memory mismatch at 0x%04X: expected 0x%02X, got 0x%02X\n", 
                   test->final.ram[i].address, test->final.ram[i].value, actual);
            passed = false;
        }
    }
    
    return passed;
}

void free_test_case(test_case *test) {
    free(test->initial.ram);
    free(test->final.ram);
    free((void*)test->name);
    free(test);
}

// run all tests in a file
void run_test_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("failed to open test file: %s\n", filename);
        return;
    }

    // read file content
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* content = malloc(length + 1);
    fread(content, 1, length, file);
    content[length] = '\0';
    fclose(file);

    // parse json array
    cJSON* json = cJSON_Parse(content);
    free(content);
    
    if (!json) {
        printf("failed to parse json: %s\n", cJSON_GetErrorPtr());
        return;
    }

    int total_tests = 0;
    int passed_tests = 0;

    // create cpu instance
    cpu gameboy;
    memset(&gameboy, 0, sizeof(cpu));
    cpu_init(&gameboy.registers);
    cpu_init_test(&gameboy.registers);
    bus_init(&gameboy.bus);

    // get opcode from filename
    const char* base_filename = strrchr(filename, '/');
    if (base_filename) {
        base_filename++; 
    } else {
        base_filename = filename;
    }
    
    // iterate through all tests in file
    cJSON* test_json;
    cJSON_ArrayForEach(test_json, json) {
        test_case* test = parse_test(test_json);
        
        // setup initial state
        setup_cpu_state(&gameboy, test);
        
        // execute one instruction
        cpu_step(&gameboy);
        
        // verify final state
        bool passed = verify_cpu_state(&gameboy, test);
        if (passed) {
            passed_tests++;
        } else {
            printf("Failed test in %s: %s\n", base_filename, test->name);
        }
        
        total_tests++;
        free_test_case(test);
    }

    // only print output if there were failures
    if (passed_tests != total_tests) {
        printf("%s: %d/%d passed\n", base_filename, passed_tests, total_tests);
    } else {
        printf("%s: All tests passed\n", base_filename);
    }
    
    // cleanup
    cJSON_Delete(json);
    bus_free(&gameboy.bus);
}