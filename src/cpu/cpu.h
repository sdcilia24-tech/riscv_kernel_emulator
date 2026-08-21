#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <memory/memory.h>

#define X_LEN 32

#define R_TYPE 0b0110011
#define I_TYPE 0b0010011
#define B_TYPE 0b1100011
#define STORE_MEMORY_TYPE 0b0100011
#define LOAD_MEMORY_TYPE 0b0000011
#define JUMP_AND_LINK_TYPE 0b1101111
#define JUMP_AND_LINK_REG_TYPE 0b1100111
#define U_TYPE_NO_PC_INC 0b0110111
#define U_TYPE_W_PC_INC 0b0010111

typedef struct {
    uint32_t registers[X_LEN];
    mem_t *memory;
    uint32_t pc;
} cpu_t;

void emulate_instruction(cpu_t *cpu, uint32_t ins);
void cpu_loop_for_testing(cpu_t *cpu, int num_cycles, char* func_name);
void cpu_loop(cpu_t *cpu);
void fetch_decode_execute(cpu_t *cpu);
void cpu_throw_fatal(int err_type);
#endif