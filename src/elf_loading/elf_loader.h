#ifndef LOADER_H
#define LOADER_H
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include <stdbool.h>

#include "cpu/cpu.h"
#include "memory/memory.h"


# define ELF_NIDENT	16
# define RISCV32 0xF3
# define EV_CURRENT	(1)

# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture

#define HEADER_BLOCK_32_BIT 52 // size of memory of the elf header for 32 bit architecture

typedef uint16_t Elf32_Half;	// Unsigned half int
typedef uint32_t Elf32_Off;	    // Unsigned offset
typedef uint32_t Elf32_Addr;	// Unsigned address
typedef uint32_t Elf32_Word;	// Unsigned int
typedef int32_t  Elf32_Sword;	// Signed int

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};

typedef struct {
    uint8_t *pointer;
    long FILE_SIZE;
} file_info_t;

typedef struct {
	uint8_t		e_ident[ELF_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    uint32_t program_header_array[8];
} program_header;

enum ELF_Program_Info {
	p_type = 0,
	p_offset = 1,
	p_vaddr = 2,
	p_paddr = 3,
	p_filesz = 4,
	p_memsz = 5,
	p_flags = 6,
	p_align = 7
};

enum Elf_Ident {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
	EI_CLASS	= 4, // Architecture (32/64)
	EI_DATA		= 5, // Byte Order
	EI_VERSION	= 6, // ELF Version
	EI_OSABI	= 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		= 9  // Padding
};

file_info_t flatten_file(const char *file);
bool header_check(Elf32_Ehdr *header);
bool is_supported(Elf32_Ehdr *header);
Elf32_Ehdr populate_header(file_info_t *info);
program_header populate_pgrm_header(Elf32_Ehdr *header, file_info_t *info, int index);
void load_to_memory(program_header *p_h, mem_t *memory, file_info_t info);
void load_programs(Elf32_Ehdr header, file_info_t info, mem_t *memory);
void set_cpu_pc(cpu_t *cpu, Elf32_Ehdr *header);
#endif