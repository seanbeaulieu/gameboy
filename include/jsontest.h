#ifndef SM83_TEST_H
#define SM83_TEST_H

#include <stdbool.h>
#include "../include/cpu.h"
#include <cjson/cJSON.h>

// test case structure
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

// core testing functions
void run_test_file(const char* filename);

// helper functions
test_case* parse_test(cJSON* json);
void setup_cpu_state(cpu *cpu, test_case *test);
bool verify_cpu_state(cpu *cpu, test_case *test);
void free_test_case(test_case *test);

#endif // SM83_TEST_H