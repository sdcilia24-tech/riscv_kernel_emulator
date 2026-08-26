#include <stdint.h>
#include <stdbool.h>
#include <string.h>


/*
file that defines ~200kB of memory and a segment structure to keep track
of relevent metadata
*/
#define MEM_LEN 0x20000


#define PF_X 0x1 // execute flag
#define PF_W 0x2 // write flag
#define PF_R 0x4 // read flag

typedef struct {
    bool loadable; // is the current segment loadable
    uint32_t flag; // permission flag
    uint32_t start; // starting point
    uint32_t end; // ending point
} p_loc_t;

typedef struct 
{
    p_loc_t segment_info; // info about the segemnt 
    int num_segments; // number of segments 
    uint8_t map[MEM_LEN]; //256 kB 
    p_loc_t *segments; // array of segments 

} mem_t;

/*
will initalize the memory to a known state
*/
mem_t init_memory()
{   
    mem_t memory;
    p_loc_t p;
    memory.num_segments = 0;
    memset(&memory.map, 0, sizeof(memory.map));
    memory.segments = NULL;
    p.start = 0;
    p.end = 0;
    p.loadable = false;
    memory.segment_info = p;
    return memory;
}

/*
    @param *memory: the memory structure
    @param addr: write address
    @param word: word to write

    writes a given word to a given address
*/
void write_word(mem_t *memory, uint32_t addr, uint32_t word) 
{
    uint8_t byte_0 = word & 0x000000FF; // lowest byte
    uint8_t byte_1 = (word & 0x0000FF00) >> 8;
    uint8_t byte_2 = (word & 0x00FF0000) >> 16;
    uint8_t byte_3 = (word & 0xFF000000) >> 24; // highest byte 

    memory -> map[addr] = byte_0;
    memory -> map[addr + 1] = byte_1;
    memory -> map[addr + 2] = byte_2;
    memory -> map[addr + 3] = byte_3;
}

/*
    @param *memory: the memory structure
    @param addr: write address
    
    will read a word from the given memory address
*/
uint32_t read_word(mem_t *memory, uint32_t addr)
{
    uint32_t byte_0 = (memory -> map[addr]);
    uint32_t byte_1 = (memory -> map[addr + 1] << 8);
    uint32_t byte_2 = (memory -> map[addr + 2] << 16);
    uint32_t byte_3 = (memory -> map[addr + 3]) << 24;
    return (byte_0 | byte_1 | byte_2 | byte_3);
}

/*
    @param *memory: the memory structure
    @param addr: write address
    @param word: word to write

    will write half of the passed word to memory at the given address
*/
void write_half(mem_t *memory, uint32_t addr, uint32_t word)
{

    uint8_t byte_0 = word & 0x000000FF; // lowest byte
    uint8_t byte_1 = (word & 0x0000FF00) >> 8;

    memory -> map[addr] = byte_0;
    memory -> map[addr + 1] = byte_1;

}

/*
    @param *memory: the memory structure
    @param addr: write address
    
    will read half a word from memory at given address
*/
uint32_t read_half(mem_t *memory, uint32_t addr)
{
    uint32_t byte_0 = memory -> map[addr]; // lowest byte
    uint32_t byte_1 = memory -> map[addr + 1] << 8; 
    return (byte_1 | byte_0);
}


/*
    @param *memory: the memory structure
    @param addr: write address
    @param word: word to write

    will write a byte to given address
*/
void write_byte(mem_t *memory, uint32_t addr, uint32_t word)
{
    uint8_t byte_0 = word & 0x000000FF; // lowest byte
    memory -> map[addr] = byte_0;
}

/*
    @param *memory: the memory structure
    @param addr: write address
    
    will read a byte from the given address
*/
uint32_t read_byte(mem_t *memory, uint32_t addr)
{
    uint32_t byte_0 = (memory -> map[addr]);
    return byte_0; 
}
