#include "memory/memory.h"
#include "elf_loading/elf_loader.h"
#include "cpu/cpu.h"
// #include <stdio.h>
int main() {
    file_info_t info = flatten_file("C:/Users/sdcil/CProjects/riscv_kernel/src/assembly_test/fibonnaci_test.elf");
    Elf32_Ehdr header = populate_header(&info);

    program_header p_h = populate_pgrm_header(&header, &info, 1);
    mem_t memory = init_memory();
    
    cpu_t cpu = init_cpu();
    cpu.memory = &memory;
    cpu.pc = header.e_entry;
    
    load_programs(header, info, &memory);
    uint32_t addr = 0x110d4;
    
    cpu_loop_for_testing(&cpu, 30, "assem test");
    return 0;
}