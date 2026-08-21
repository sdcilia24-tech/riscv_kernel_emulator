#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "memory/memory.h"


# define ELF_NIDENT	16 // the header array 
# define RISCV32 0xF3 // architecture signature
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

enum program_type {
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PR_SHLIB = 5,
    PT_PHDR = 6,
    PT_TLS = 7,
    PT_LOOS = 0x60000000,
    PT_HIOS = 0x6fffffff,
    PT_LOPROC = 0x70000000,
    PT_HIPROC = 0x7fffffff
};

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};

typedef struct {
    uint8_t *pointer; // pointer to the files entry point as it will need to be allocated
    long FILE_SIZE; // size of the file
} file_info_t;

typedef struct {
	uint8_t		e_ident[ELF_NIDENT];
	Elf32_Half	e_type; // elf type
	Elf32_Half	e_machine; // system architecture
	Elf32_Word	e_version; // elf version
	Elf32_Addr	e_entry; // entry point
	Elf32_Off	e_phoff; // program offset
	Elf32_Off	e_shoff;  
	Elf32_Word	e_flags; // flags
	Elf32_Half	e_ehsize; // elf header size
	Elf32_Half	e_phentsize; // program header entry size
	Elf32_Half	e_phnum; // number of program headers 
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


/*
    @param *file: a string corresponding to the file path

    will take the file, open it calculate the total size, pass the fle to the heap (as they will be large)
    set the pointer to the entry point of the file so we can iterate through it close the file one we have access and the size
    and return 
*/
file_info_t flatten_file(const char *file) 
{   
    file_info_t info;
    FILE *f = fopen(file, "rb");
    if (f == NULL)
    {
        printf("UNABLE TO OPEN FILE \n");
        exit(EXIT_FAILURE);
    }
    fseek(f, 0, SEEK_END);

    info.FILE_SIZE = ftell(f);
    void *file_iter = malloc(info.FILE_SIZE); // pass file to the heap 

    rewind(f);
    info.pointer = (uint8_t *)file_iter;

    long num_elements = info.FILE_SIZE / sizeof(uint8_t);
    size_t s = fread(file_iter, sizeof(uint8_t), num_elements, f);
    fclose(f);
    return info;

}

/*
    @param *header: elf header

    Will ensure that the value in the header matches the expected values so that we can 
    actually execute the given file
*/

bool header_check(Elf32_Ehdr *header)
{
    if (!header)
    {
        return false;
    }
    if (header -> e_ident[EI_MAG0] != ELFMAG0)
    {
        return false;
    }
    if (header -> e_ident[EI_MAG1] != ELFMAG1)
    {
        return false;
    }
    if (header -> e_ident[EI_MAG2] != ELFMAG2)
    {
        return false;
    }
    else
    {
        return (header -> e_ident[EI_MAG3] == ELFMAG3);
    }

}

/*
    @param *header: elf header
    
    similar to the header check, but instead we are checking the machine 
    and the elf type 
*/

bool is_supported(Elf32_Ehdr *header)
{
    if (header -> e_ident[EI_CLASS] != ELFCLASS32)
    {
        return false;
    }
    if (header -> e_ident[EI_DATA] != ELFDATA2LSB)
    {
        return false;
    }
    if (header -> e_machine != RISCV32)
    {
        return false;
    }
    if (header -> e_ident[EI_VERSION] != EV_CURRENT)
    {
        return false;
    }
    if (header -> e_type != ET_REL && header -> e_type != ET_EXEC) 
    {
		return false;
    }
    return true;
}

/*
    @param *arr: array of bytes
    @param x: index

    converts two bytes into a little endian half
*/
uint16_t half_to_little_endian(uint8_t *arr, int x)
{
    return (arr[x - 1] | (arr[x] << 8));
}

/*
    @param arr: array of bytes
    @param x: index

    converts four bytes into a little endian word
*/
uint32_t word_to_little_endian(uint8_t *arr, int x)
{
    return (arr[x] << 24) | (arr[x - 1] << 16) | (arr[x - 2] << 8) | (arr[x - 3]); 
}

/*
    @param *info: file information

    populate the elf header with the value parsed from the flattened elf file, we know that the beginning of the elf header
    (the part that remains the same) occupies the first 52 bytes of every elf file hence we can populate each header the same way
*/
Elf32_Ehdr populate_header(file_info_t *info)
{   Elf32_Ehdr header;
    uint8_t *file_runner = info -> pointer; // shallow copy 
    for (int i = 0; i < 16; i++)
    {
        header.e_ident[i] = file_runner[i];
        continue;
    }

    if (!header_check(&header))
    {   
        printf("elf header does not match required type\n");
        exit(EXIT_FAILURE);
    }

    uint16_t type = half_to_little_endian(file_runner, 17);
    header.e_type = type;

    uint16_t machine = half_to_little_endian(file_runner, 19);
    header.e_machine = machine;

    uint32_t version = word_to_little_endian(file_runner, 23);
    header.e_version = version;

    uint32_t entry = word_to_little_endian(file_runner, 27);
    header.e_entry = entry;

    uint32_t p_off = word_to_little_endian(file_runner, 31);
    header.e_phoff = p_off;

    uint32_t s_hoff = word_to_little_endian(file_runner, 35);
    header.e_shoff = s_hoff;

    uint32_t flags = word_to_little_endian(file_runner, 39);
    header.e_flags = flags;

    uint16_t ehsize = half_to_little_endian(file_runner, 41);
    header.e_ehsize = ehsize;

    uint16_t phent_size = half_to_little_endian(file_runner, 43);
    header.e_phentsize = phent_size;

    uint16_t phnum = half_to_little_endian(file_runner, 45);
    header.e_phnum = phnum;

    uint16_t shent_size = half_to_little_endian(file_runner, 47);
    header.e_shentsize = shent_size;

    uint16_t shnum = half_to_little_endian(file_runner, 49);
    header.e_shnum = shnum;

    uint16_t shtrndx = half_to_little_endian(file_runner, 51);
    header.e_shstrndx = shtrndx;
    return header;
}

/*
    @param *header: elf header
    @param *info: file info
    @param index: the current program number as seen in the file order (ie first seen = 0, second = 1 etc)

    populates the program header with the neccesary 32 bit little endian values from the elf file
*/
program_header populate_pgrm_header(Elf32_Ehdr *header, file_info_t *info, int index)
{
    uint8_t *header_entry = ((info -> pointer) + (header -> e_phoff));
    program_header p_head;
    if (index >= header -> e_phnum || index < 0)
    {
        exit(EXIT_FAILURE);
    }
    int scale = index * header -> e_phentsize;
    int current_info = 7;
    int current_byte = 31;
    while (current_info >= 0 && current_byte >= 0)
    {
        p_head.program_header_array[current_info] = word_to_little_endian(header_entry, current_byte + scale);
        current_byte -= 4;
        current_info -= 1;
    }

    return p_head;
}


/*
    @param *p_h: program header
    @param *memory: pointer to memory struct
    @param info: the file information

    will check to see if the current program is loadable, and if so we will load the program header
    into memory and then assign the program flags with the corresponding index in the parallel memory
*/
void load_to_memory(program_header *p_h, mem_t *memory, file_info_t info)
{

    if (p_h -> program_header_array[p_type] != PT_LOAD)
    {
        memory -> segment_info.loadable = false;
    }
    else  
    {
        memory -> segment_info.loadable = true;

        uint32_t alignment = p_h -> program_header_array[p_align];

        uint32_t addr = p_h -> program_header_array[p_vaddr]; // starting address
        uint32_t mem_size = p_h -> program_header_array[p_memsz];
        
        uint32_t p_flag = p_h -> program_header_array[p_flags];

        uint32_t file_size = p_h -> program_header_array[p_filesz];
        uint32_t memory_offset = p_h -> program_header_array[p_offset];

        uint32_t end = addr + mem_size; // ending address

        memory -> segment_info.start = addr;
        memory -> segment_info.end = end;
        memory -> segment_info.flag = p_flag;

        uint8_t *file_runner = info.pointer + memory_offset;

        for (int i = 0; i < mem_size; i++)
        {
            if (i >= file_size)
            {
                memory -> map[addr + i] = 0;
            }
            else  
            {
                memory -> map[addr + i] = file_runner[i];
            }
        }
    }
}


/*
    @param header: elf header
    @param info: file info
    @param *memory: memory structure

    will load the current program to memory and will keep track of the entry and exit point of the programs
    if the segment is loadable in the segments structure 
*/
void load_programs(Elf32_Ehdr header, file_info_t info, mem_t *memory)
{

    memory -> segments = malloc(header.e_phnum * sizeof(p_loc_t));
    for (int i = 0; i < header.e_phnum; i++)
    {
        program_header p_h = populate_pgrm_header(&header, &info, i);
        load_to_memory(&p_h, memory, info);
        if (memory -> segment_info.loadable)
        {
            memory -> segments[memory -> num_segments] = memory -> segment_info;
            memory -> num_segments++;
        }

    }
    free(info.pointer);
}


