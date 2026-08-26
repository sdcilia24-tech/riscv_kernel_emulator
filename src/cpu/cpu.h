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

typedef struct 
{
    long old_pc;
    long new_pc;

    long old_reg_val;
    long new_reg_val;

    uint32_t opcode;

} cpu_info_t;

typedef struct {
    uint32_t registers[X_LEN];
    mem_t *memory;
    uint32_t pc;
    cpu_info_t data; // cpu data
    cpu_info_t *logs; // array of logs 
} cpu_t;

void emulate_instruction(cpu_t *cpu, uint32_t ins);
int32_t extend_n_bit_sign(int32_t x, int len);
void cpu_loop_for_testing(cpu_t *cpu, int num_cycles, char* func_name);
void cpu_loop(cpu_t *cpu);
void fetch_decode_execute(cpu_t *cpu);
void cpu_throw_fatal(int err_type);
cpu_info_t trace_cpu(cpu_t *cpu, uint32_t ins, uint32_t *old_reg, uint32_t old_pc);
cpu_t init_cpu();
#endif