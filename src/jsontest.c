#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/jsontest.h"

static char* safe_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char* new_str = malloc(len + 1);
    if (!new_str) return NULL;
    strcpy(new_str, str);
    return new_str;
}


// load and parse a single test from json
test_case* parse_test(cJSON* json) {
    // validate input
    if (!json) {
        printf("null json object passed to parse_test\n");
        return NULL;
    }

    // allocate test case structure
    test_case* test = malloc(sizeof(test_case));
    if (!test) {
        printf("failed to allocate memory for test case\n");
        return NULL;
    }
    
    // initialize all fields to 0
    memset(test, 0, sizeof(test_case));
    
    // parse test name
    cJSON* name = cJSON_GetObjectItem(json, "name");
    if (name && name->valuestring) {
        test->name = safe_strdup(name->valuestring);
    } else {
        test->name = safe_strdup("unnamed test");
    }
    
    if (!test->name) {
        printf("failed to allocate memory for test name\n");
        free(test);
        return NULL;
    }

    //
    // parse initial state
    //
    cJSON* initial = cJSON_GetObjectItem(json, "initial");
    if (!initial) {
        printf("missing initial state in test\n");
        goto cleanup_name;
    }

    // get initial registers
    cJSON* pc = cJSON_GetObjectItem(initial, "pc");
    cJSON* sp = cJSON_GetObjectItem(initial, "sp");
    cJSON* a = cJSON_GetObjectItem(initial, "a");
    cJSON* b = cJSON_GetObjectItem(initial, "b");
    cJSON* c = cJSON_GetObjectItem(initial, "c");
    cJSON* d = cJSON_GetObjectItem(initial, "d");
    cJSON* e = cJSON_GetObjectItem(initial, "e");
    cJSON* f = cJSON_GetObjectItem(initial, "f");
    cJSON* h = cJSON_GetObjectItem(initial, "h");
    cJSON* l = cJSON_GetObjectItem(initial, "l");

    // validate all registers exist
    if (!pc || !sp || !a || !b || !c || !d || !e || !f || !h || !l) {
        printf("missing register in initial state\n");
        goto cleanup_name;
    }

    // store initial register values
    test->initial.pc = pc->valueint & 0xFFFF;  // ensure 16-bit
    test->initial.sp = sp->valueint & 0xFFFF;
    test->initial.a = a->valueint & 0xFF;      // ensure 8-bit
    test->initial.b = b->valueint & 0xFF;
    test->initial.c = c->valueint & 0xFF;
    test->initial.d = d->valueint & 0xFF;
    test->initial.e = e->valueint & 0xFF;
    test->initial.f = f->valueint & 0xFF;
    test->initial.h = h->valueint & 0xFF;
    test->initial.l = l->valueint & 0xFF;

    // parse initial ram array
    cJSON* ram = cJSON_GetObjectItem(initial, "ram");
    if (!ram || !cJSON_IsArray(ram)) {
        printf("missing or invalid ram array in initial state\n");
        goto cleanup_name;
    }

    // get size of ram array
    test->initial.ram_size = cJSON_GetArraySize(ram);
    if (test->initial.ram_size > 0) {
        // allocate ram array
        test->initial.ram = malloc(sizeof(test->initial.ram[0]) * test->initial.ram_size);
        if (!test->initial.ram) {
            printf("failed to allocate memory for initial ram\n");
            goto cleanup_name;
        }

        // parse each ram entry
        for (int i = 0; i < test->initial.ram_size; i++) {
            cJSON* ram_entry = cJSON_GetArrayItem(ram, i);
            if (!ram_entry || !cJSON_IsArray(ram_entry) || cJSON_GetArraySize(ram_entry) != 2) {
                printf("invalid ram entry at index %d\n", i);
                goto cleanup_initial_ram;
            }

            cJSON* addr = cJSON_GetArrayItem(ram_entry, 0);
            cJSON* value = cJSON_GetArrayItem(ram_entry, 1);

            if (!addr || !value) {
                printf("missing address or value in ram entry %d\n", i);
                goto cleanup_initial_ram;
            }

            // validate address is within gameboy memory range
            uint32_t temp_addr = addr->valueint;
            if (temp_addr > 0xFFFF) {
                printf("invalid ram address 0x%04X in initial state\n", temp_addr);
                goto cleanup_initial_ram;
            }

            test->initial.ram[i].address = (uint16_t)temp_addr;
            test->initial.ram[i].value = value->valueint & 0xFF;  // ensure 8-bit
        }
    }

    //
    // parse final state
    //
    cJSON* final = cJSON_GetObjectItem(json, "final");
    if (!final) {
        printf("missing final state in test\n");
        goto cleanup_initial_ram;
    }

    // get final registers (same process as initial)
    pc = cJSON_GetObjectItem(final, "pc");
    sp = cJSON_GetObjectItem(final, "sp");
    a = cJSON_GetObjectItem(final, "a");
    b = cJSON_GetObjectItem(final, "b");
    c = cJSON_GetObjectItem(final, "c");
    d = cJSON_GetObjectItem(final, "d");
    e = cJSON_GetObjectItem(final, "e");
    f = cJSON_GetObjectItem(final, "f");
    h = cJSON_GetObjectItem(final, "h");
    l = cJSON_GetObjectItem(final, "l");

    // validate all registers exist
    if (!pc || !sp || !a || !b || !c || !d || !e || !f || !h || !l) {
        printf("missing register in final state\n");
        goto cleanup_initial_ram;
    }

    // store final register values
    test->final.pc = pc->valueint & 0xFFFF;
    test->final.sp = sp->valueint & 0xFFFF;
    test->final.a = a->valueint & 0xFF;
    test->final.b = b->valueint & 0xFF;
    test->final.c = c->valueint & 0xFF;
    test->final.d = d->valueint & 0xFF;
    test->final.e = e->valueint & 0xFF;
    test->final.f = f->valueint & 0xFF;
    test->final.h = h->valueint & 0xFF;
    test->final.l = l->valueint & 0xFF;

    // parse final ram array
    ram = cJSON_GetObjectItem(final, "ram");
    if (!ram || !cJSON_IsArray(ram)) {
        printf("missing or invalid ram array in final state\n");
        goto cleanup_initial_ram;
    }

    // get size of final ram array
    test->final.ram_size = cJSON_GetArraySize(ram);
    if (test->final.ram_size > 0) {
        // allocate final ram array
        test->final.ram = malloc(sizeof(test->final.ram[0]) * test->final.ram_size);
        if (!test->final.ram) {
            printf("failed to allocate memory for final ram\n");
            goto cleanup_initial_ram;
        }

        // parse each final ram entry
        for (int i = 0; i < test->final.ram_size; i++) {
            cJSON* ram_entry = cJSON_GetArrayItem(ram, i);
            if (!ram_entry || !cJSON_IsArray(ram_entry) || cJSON_GetArraySize(ram_entry) != 2) {
                printf("invalid ram entry at index %d in final state\n", i);
                goto cleanup_final_ram;
            }

            cJSON* addr = cJSON_GetArrayItem(ram_entry, 0);
            cJSON* value = cJSON_GetArrayItem(ram_entry, 1);

            if (!addr || !value) {
                printf("missing address or value in final ram entry %d\n", i);
                goto cleanup_final_ram;
            }

            uint32_t temp_addr = addr->valueint;
            if (temp_addr > 0xFFFF) {
                printf("invalid ram address 0x%04X in final state\n", temp_addr);
                goto cleanup_final_ram;
            }

            test->final.ram[i].address = (uint16_t)temp_addr;
            test->final.ram[i].value = value->valueint & 0xFF;
        }
    }

    // successfully parsed everything
    return test;

    // cleanup labels for error handling
cleanup_final_ram:
    free(test->final.ram);
cleanup_initial_ram:
    free(test->initial.ram);
cleanup_name:
    free((void*)test->name);
    free(test);
    return NULL;
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
    
    // set memory values - print for debugging
    for (int i = 0; i < test->initial.ram_size; i++) {
        uint16_t addr = test->initial.ram[i].address;
        uint8_t value = test->initial.ram[i].value;
        bus_write8(&cpu->bus, addr, value);
        // Add debug print
        // printf("Writing 0x%02X to address 0x%04X\n", value, addr);
    }
}

// verify cpu state matches expected final state
bool verify_cpu_state(cpu *cpu, test_case *test) {
    if (!cpu || !test) {
        printf("null pointer passed to verify_cpu_state\n");
        return false;
    }

    bool passed = true;
    
    // check registers
    if (cpu->registers.pc != test->final.pc) {
        // printf("pc mismatch: expected 0x%04X, got 0x%04X\n", test->final.pc, cpu->registers.pc);
        passed = false;
    }
    if (cpu->registers.sp != test->final.sp) {
        // printf("sp mismatch: expected 0x%04X, got 0x%04X\n", test->final.sp, cpu->registers.sp);
        passed = false;
    }
    if (cpu->registers.a != test->final.a) {
        // printf("a mismatch: expected 0x%02X, got 0x%02X\n", test->final.a, cpu->registers.a);
        passed = false;
    }
    if (cpu->registers.b != test->final.b) {
        // printf("b mismatch: expected 0x%02X, got 0x%02X\n", test->final.b, cpu->registers.b);
        passed = false;
    }
    if (cpu->registers.c != test->final.c) {
        // printf("c mismatch: expected 0x%02X, got 0x%02X\n", test->final.c, cpu->registers.c);
        passed = false;
    }
    if (cpu->registers.d != test->final.d) {
        // printf("d mismatch: expected 0x%02X, got 0x%02X\n", test->final.d, cpu->registers.d);
        passed = false;
    }
    if (cpu->registers.e != test->final.e) {
        // printf("e mismatch: expected 0x%02X, got 0x%02X\n", test->final.e, cpu->registers.e);
        passed = false;
    }
    
    uint8_t actual_f = flags_register_to_byte(cpu->registers.f);
    if (actual_f != test->final.f) {
        // printf("f mismatch: expected 0x%02X, got 0x%02X\n", test->final.f, actual_f);
        passed = false;
    }
    
    if (cpu->registers.h != test->final.h) {
        // printf("h mismatch: expected 0x%02X, got 0x%02X\n", test->final.h, cpu->registers.h);
        passed = false;
    }
    if (cpu->registers.l != test->final.l) {
        // printf("l mismatch: expected 0x%02X, got 0x%02X\n", test->final.l, cpu->registers.l);
        passed = false;
    }
    
    // check memory with validation
    for (int i = 0; i < test->final.ram_size; i++) {
        uint8_t actual = bus_read8(&cpu->bus, test->final.ram[i].address);
        if (actual != test->final.ram[i].value) {
            // printf("memory mismatch at 0x%04X: expected 0x%02X, got 0x%02X\n", 
                //    test->final.ram[i].address, test->final.ram[i].value, actual);
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
void run_test_file(const char* filename, cpu *gameboy) {
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
    if (!content) {
        fclose(file);
        return;
    }
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

    // get opcode from filename
    const char* base_filename = strrchr(filename, '/');
    if (base_filename) {
        base_filename++; 
    } else {
        base_filename = filename;
    }
    
    // iterate through tests
    cJSON* test_json;
    cJSON_ArrayForEach(test_json, json) {
        test_case* test = parse_test(test_json);
        if (test) {
            setup_cpu_state(gameboy, test);
            cpu_step(gameboy);
            if (verify_cpu_state(gameboy, test)) {
                passed_tests++;
            } else {
                // printf("Failed test in %s: %s\n", base_filename, test->name);
            }
            total_tests++;
            free_test_case(test);
        }
    }

    printf("%s: %d/%d tests passed\n", base_filename, passed_tests, total_tests);
    cJSON_Delete(json);
}