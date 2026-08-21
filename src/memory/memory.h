#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

#define MEM_LEN 0x20000
#define PF_X 0x1 // execute
#define PF_W 0x2 // write
#define PF_R 0x4 // read


typedef struct {
    bool loadable;
    uint32_t flag;
    uint32_t start;
    uint32_t end;
} p_loc_t;


typedef struct 
{
    p_loc_t segment_info;
    int num_segments;
    uint8_t map[MEM_LEN]; // 256 kB
    uint32_t program_map[MEM_LEN];
    p_loc_t *segments;

} mem_t;


void write_word(mem_t *memory, uint32_t addr, uint32_t word);
uint32_t read_word(mem_t *memory, uint32_t addr);
void write_half(mem_t *memory, uint32_t addr, uint32_t word);
uint32_t read_half(mem_t *memory, uint32_t addr);
void write_byte(mem_t *memory, uint32_t addr, uint32_t word);
uint32_t read_byte(mem_t *memory, uint32_t addr);
bool has_write(mem_t *memory, uint32_t addr);
bool has_exec(mem_t *memory, uint32_t addr);
bool has_read(mem_t *memory, uint32_t addr);
mem_t init_memory();
#endif