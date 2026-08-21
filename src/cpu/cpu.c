#include <stdio.h>
#include "memory/memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define X_LEN 32

// Opcode typing for easy refereence
#define R_TYPE 0b0110011
#define I_TYPE 0b0010011
#define B_TYPE 0b1100011
#define STORE_MEMORY_TYPE 0b0100011
#define LOAD_MEMORY_TYPE 0b0000011
#define JUMP_AND_LINK_TYPE 0b1101111
#define JUMP_AND_LINK_REG_TYPE 0b1100111
#define U_TYPE_NO_PC_INC 0b0110111
#define U_TYPE_W_PC_INC 0b0010111


// error typing
#define OPCODE_ERR 6767
#define NO_EXEC 6768
#define NO_WRITE 6769
#define NO_READ 6770
#define ILLEGAL_MEM_ACCESS 6771
#define ILLEGAL_FUNCT7 6772
#define ILLEGAL_FUNCT3 6773

typedef struct {

    long old_pc; // old program counter value
    long new_pc; // new program counter value

    long register_affected; // the register affected
    long old_reg_val; // old value in register
    long new_reg_val; // new value in register

    uint32_t opcode;

} cpu_info_t;

typedef struct {

    uint32_t registers[X_LEN]; // cpu registers
    mem_t *memory; // cpu reference to memory 
    uint32_t pc; // program counter 
    cpu_info_t data; // cpu data

} cpu_t;

cpu_t init_cpu()
{
    cpu_t cpu;
    cpu_info_t inf;
    memset(&cpu.registers, 0, sizeof(cpu.registers));
    cpu.memory = NULL;
    inf.new_pc = 0;
    inf.old_pc = 0;
    inf.new_reg_val = 0;
    inf.old_reg_val = 0;
    inf.opcode = 0;
    inf.register_affected = 0;
    return cpu;
}

/*
    @param to_copy: array to copy
    @param len: length of the array
    Helper function to create a deep copy of an array 
*/
uint32_t* deep_copy(uint32_t *to_copy, int len)
{
    if (to_copy == NULL)
    {
        return NULL;
    }
    else

    {
        uint32_t *deep_copy = malloc(sizeof(uint32_t) * len);
        if (deep_copy == NULL)
        {
            return NULL;
        }
        for (int i = 0; i < len; i++)
        {
            deep_copy[i] = to_copy[i];
        }

        return deep_copy;
    }
}
 /*
    @param x: the integer that you want to sign extend
    @param length: the length of the integer (0101 HAS LENGTH FOUR)
    Will take an unsigned integer and produce the sign extended version of 
    the integer. Begin by shifting and taking the top most bit (len - 1) then if 
    its zero, the integer is positive and we are finished, otherwise, pad the integer with
    leading ones.
 */
int32_t extend_n_bit_sign(int32_t x, int len)
{   if (len > 32)
    {
        return x;
    }
    uint8_t sign = (x >> (len - 1)) & 1;
    if (sign == 0) 
    {
        return x;
    }
    else
    {
        uint32_t flipper = ~0; // generate 11111.... 
        return ((flipper << len) | x); // mask the sign
    }
}
/*
    @param err_type: error types SEE LINE 20 FOR TYPE NAMES
    @param addr: the adress/function hex code at which the error occured
    This is the way of throwing fatal errors, that will print the error message 
    to the console and then will exit the program essentially killing the process
*/
void cpu_throw_fatal(int err_type, uint32_t addr)
{
    if (err_type == OPCODE_ERR)
    {
        printf("ILLEGAL INSTRUCTION ATTEMPT AT: %X \n", addr);
    }
    else if (err_type == NO_EXEC)
    {
        printf("PERMISSION ERROR: ILLEGAL EXECUTE ATTEMPT AT: '%X' \n", addr);
    }
    else if (err_type == NO_WRITE)
    {
        printf("PERMISSION ERROR: ILLEGAL WRITE ATTEMPT AT: '%X' \n", addr);
    }
    else if (err_type == NO_READ)
    {
        printf("PERMISSION ERROR: ILLEGAL READ ATTEMPT AT: '%X' \n", addr);
    }
    else if (err_type == ILLEGAL_MEM_ACCESS)
    {
        printf("MEMORY ERROR: ILLEGAL FETCH ATTEMPT AT: '%X' \n", addr);
    }
    else if (err_type == ILLEGAL_FUNCT7)
    {
        printf("ILLEGAL INSTRUCTION: TYPE FUNCT 7: '%X'\n", addr);
    }
    else if (err_type == ILLEGAL_FUNCT3)
    {
        printf("ILLEGAL INSTRUCTION: TYPE FUNCT 3: '%X'\n", addr);
    }
    exit(EXIT_FAILURE);
}

/*
    @param *cpu: pointer to cpu_t structure
    @param funct3: the bits 14 - 12 in the 32 bit instruction
    @param funct7: the bits 31 - 25
    @param ins: the 32 bit instruction
    
    This function implemets the R type instructions of the RISCV which are 
    primarily the arithemtic types, to see the particulars about the function 
    view the nline comments
*/
void execute_r_type(cpu_t *cpu, uint32_t funct3, uint32_t funct7, uint32_t ins)
{
    uint8_t rs2 = (ins >> 20) & 0x1F; // bit 24 - 20
    uint8_t rs1 = (ins >> 15) & 0x1F; // bits 19 - 15
    uint32_t rs1_unsigned_val = cpu -> registers[rs1]; // val of rs1 reg
    uint32_t rs2_unsigned_val = cpu -> registers[rs2]; // val of rs2 reg
    uint8_t rd = (ins >> 7) & 0x1F; // bits 11 - 7

    switch (funct3) 
    {
        default:
            cpu_throw_fatal(ILLEGAL_FUNCT3, funct3);
            break;
        case 0x0: // add sub
            if (funct7 == 0x0)
            {
                cpu -> registers[rd] = rs2_unsigned_val + rs1_unsigned_val;
            }
            else if (funct7 == 0x20) 
            {
                cpu -> registers[rd] = rs1_unsigned_val - rs2_unsigned_val;
            }
            else 
            {
                cpu_throw_fatal(ILLEGAL_FUNCT7, funct7);
            }
            break;

        case 0x4: cpu -> registers[rd] = rs1_unsigned_val ^ rs2_unsigned_val; break; // xor

        case 0x6: cpu -> registers[rd] = rs1_unsigned_val | rs2_unsigned_val; break; // or
        
        case 0x7: cpu -> registers[rd] = rs1_unsigned_val & rs2_unsigned_val; break; // and

        case 0x1: cpu -> registers[rd] = rs1_unsigned_val << rs2_unsigned_val; break; //shift left logical
        
        case 0x5: 
            if (funct7 == 0x0) 
            {
                cpu -> registers[rd] = rs1_unsigned_val >> (rs2_unsigned_val % 32); // shift right logical
            }
            else if (funct7 == 0x20) 
            {
                cpu -> registers[rd] = (int32_t)rs1_unsigned_val >> ((int32_t)rs2_unsigned_val % 32); // shift right arithemtic
            }
            else
            {
                cpu_throw_fatal(ILLEGAL_FUNCT7, funct7);
            }
            break;

        case 0x2: cpu -> registers[rd] = ((int32_t)rs1_unsigned_val < (int32_t)rs2_unsigned_val) ? 1 : 0; break; // bool rs1 < rs2

        case 0x3: cpu -> registers[rd] = (rs1_unsigned_val < rs2_unsigned_val) ? 1 : 0; break; // bool rs1 < rs2 unsigned

    }
}
/*
    @param *cpu: pointer to cpu_t structure
    @param funct3: the bits 14 - 12 in the 32 bit instruction
    @param funct7: the bits 31 - 25
    @param ins: 32 bit instruction

    Implements the i type instructions which are of the immediate type
    these will use an encoded value "imm" to perform its operations rather than
    register values
*/
void execute_i_type(cpu_t *cpu, uint32_t funct3, uint32_t funct7, uint32_t ins)
{
    int32_t imm = extend_n_bit_sign(ins >> 20, 12);
    uint32_t rs1 = (ins >> 15) & 0x1F;
    uint32_t rd = (ins >> 7) & 0x1F;
    int32_t rs1_signed_val = cpu -> registers[rs1];
    uint32_t rs1_unsigned_val = cpu -> registers[rs1];
    switch (funct3) 
    {
        default:
            cpu_throw_fatal(ILLEGAL_FUNCT3, funct3);
            break;
        case 0x0:
            cpu -> registers[rd] = rs1_signed_val + imm; // rs1 + imm
            break;
        case 0x4:
            cpu -> registers[rd] = rs1_signed_val ^ imm; // rs1 xor imm
            break;
        case 0x6:
            cpu -> registers[rd] = rs1_signed_val | imm; // rs1 or imm
            break;
        case 0x7:
            cpu -> registers[rd] = rs1_signed_val & imm; // rs1 and imm
            break;
        case 0x1:
            cpu -> registers[rd] = rs1_unsigned_val << (imm & 0x1F); // shift rs1 left by the 5 least significant bits in imm
            break;
        case 0x5:
            if (funct7 == 0x0)
            {
                cpu -> registers[rd] = rs1_unsigned_val >> (imm & 0x1F);
            }
            else if (funct7 == 0x20)
            {
                cpu -> registers[rd] = rs1_signed_val >> (imm & 0x1F);
            }
            else
            {
                cpu_throw_fatal(ILLEGAL_FUNCT7, funct7);
            }
            break;
        case 0x2:
            cpu -> registers[rd] = (rs1_signed_val < imm) ? 1 : 0; break;

        case 0x3:
            cpu -> registers[rd] = ((uint32_t)rs1_signed_val < (uint32_t)imm) ? 1 : 0; break;
    }

}

/*
    @param *cpu: pointer to cpu_t structure
    @param funct3: the bits 14 - 12 in the 32 bit instruction
    @param ins: 32 bit instruction

    These implement the b type instructions also known as the branch types
    which will add an encoded "imm" value to whatever value the program counter
    is currently if a certain condition is true/false. 
*/
void execute_b_type(cpu_t *cpu, uint32_t funct3, uint32_t ins)
{
    uint32_t rs2 = (ins >> 20) & 0x1F; // bit 24 - 20
    uint32_t rs1 = (ins >> 15) & 0x1F; // bits 19 - 15
    uint32_t rs1_unsigned_val = cpu -> registers[rs1]; // val of rs1 reg
    uint32_t rs2_unsigned_val = cpu -> registers[rs2]; // val of rs2 reg
    int32_t rs1_signed_val = extend_n_bit_sign(rs1_unsigned_val, 32);
    int32_t rs2_signed_val = extend_n_bit_sign(rs2_unsigned_val, 32);

    uint32_t twelveth_bit = ((ins >> 31) & 0x01) << 12; // 1-bit
    uint32_t ten_to_five = ((ins >> 25) & 0x3F) << 5; // 6-bits

    uint32_t eleventh_bit = ((ins >> 7) & 0x01) << 11; // 1-bit
    uint32_t four_to_one = ((ins >> 8) & 0x0F) << 1; // 4 bits

    uint32_t imm = extend_n_bit_sign(twelveth_bit | eleventh_bit | ten_to_five | four_to_one, 13);

    switch (funct3) 
    {
        default:
            cpu_throw_fatal(ILLEGAL_FUNCT3, funct3);
            break;
        case 0x0:
            if (rs1_signed_val == rs2_signed_val)
            {
                cpu -> pc += imm;
            }
            else
            {
                cpu -> pc += 4;
            }
            break;
        case 0x1:
            if (rs1_signed_val != rs2_signed_val)
            {
                cpu -> pc += imm;
            }
            else 
            {
                cpu -> pc += 4;
            }
            break;
        case 0x4:
            if (rs1_signed_val < rs2_signed_val)
            {
                cpu -> pc += imm;
            }
            else 
            {
                cpu -> pc += 4;
            }
            break;
        case 0x5:
            if (rs1_signed_val >= rs2_signed_val)
            {
                cpu -> pc += imm;
            }
            else 
            {
                cpu -> pc += 4;
            }
            break;
        case 0x6:
            if (rs1_unsigned_val < rs2_unsigned_val)
            {
                cpu -> pc += imm;
            }
            else 
            {
                cpu -> pc += 4;
            }
            break;
        case 0x7:
            if (rs1_unsigned_val >= rs2_unsigned_val)
            {
                cpu -> pc += imm;
            }
            else 
            {
                cpu -> pc += 4;
            }
            break;
    }
}

/*
    @param *cpu: pointer to cpu_t structure
    @param funct3: the bits 14 - 12 in the 32 bit instruction
    @param ins: 32 bit instruction

    This will load a value from the memory address cpu[rs1[val]] + "imm" into the cpu's register, these require
    read permissions from memory. and can load a certain range of values including 
    a byte, half and word.
*/
void execute_load_memory(cpu_t *cpu, uint32_t funct3, uint32_t ins, uint32_t perm)
{
    int32_t imm = extend_n_bit_sign(ins >> 20, 12);
    uint32_t rs1 = (ins >> 15) & 0x1F;
    uint32_t rd = (ins >> 7) & 0x1F;
    uint32_t rs1_unsigned_val = cpu -> registers[rs1];
    uint32_t mem_addr = (rs1_unsigned_val + imm);
    if (!(perm & PF_R))
    {
        cpu_throw_fatal(NO_READ, mem_addr);
    }
    switch (funct3)
    {
        default:
            cpu_throw_fatal(ILLEGAL_FUNCT3, funct3);
            break;
        case 0x0: // load byte 
            cpu -> registers[rd] = extend_n_bit_sign(read_byte(cpu -> memory, mem_addr), 8);
            break;
        case 0x1: // load half
            cpu -> registers[rd] = extend_n_bit_sign(read_half(cpu -> memory, mem_addr), 16);
            break;
        case 0x2: // load word
            cpu -> registers[rd] = read_word(cpu -> memory, mem_addr);
            break;
        case 0x4: // load byte U
            cpu -> registers[rd] = read_byte(cpu -> memory, mem_addr);
            break;
        case 0x5: // load half U
            cpu -> registers[rd] = read_half(cpu -> memory, mem_addr);
            break;
    }
}

/*
    @param *cpu: pointer to cpu_t structure
    @param funct3: the bits 14 - 12 in the 32 bit instruction
    @param ins: 32 bit instruction

    this will store bytes/halves/words to memory at value cpu[rs1[val]] + "imm" from
    the cpu's registers -> rs2, requires write permissions
*/
void execute_store_memory(cpu_t *cpu, uint32_t funct3, uint32_t ins, uint32_t perm)
{
    uint32_t top_bits = ((ins >> 25) & 0x7F) << 5;
    uint32_t bottom_bits = (ins >> 7) & 0x1F;
    int32_t imm = extend_n_bit_sign(top_bits | bottom_bits, 12);

    uint32_t rs2 = (ins >> 20) & 0x1F; // bit 24 - 20
    uint32_t rs1 = (ins >> 15) & 0x1F; // bits 19 - 15
    uint32_t rs1_unsigned_val = cpu -> registers[rs1];

    uint32_t mem_addr = (imm + rs1_unsigned_val);
    if (!(perm & PF_W))
    {
        cpu_throw_fatal(NO_WRITE, mem_addr);
    }
    switch (funct3)
    {
        default:
            cpu_throw_fatal(ILLEGAL_FUNCT3, funct3);
            break;
        case 0x0: // write a byte at imm + rs1
            write_byte(cpu -> memory, mem_addr, cpu -> registers[rs2]);
            break;
        case 0x1: // write a half at imm + rs1
            write_half(cpu -> memory, mem_addr, cpu -> registers[rs2]);
            break;
        case 0x2: // write a word at imm + rs1
            write_word(cpu -> memory, mem_addr, cpu -> registers[rs2]);
            break;
    }
}


/*
    @param *cpu: pointer to cpu_t structure
    @param ins: 32 bit instruction

    Will emulate the passed 32 bit instruction 
*/
void emulate_instruction(cpu_t *cpu, uint32_t ins, uint32_t perm) 
{
    uint8_t opcode = ins & 0x7F; // 6 - 0

    switch (opcode) 
    {
        default:
            cpu_throw_fatal(OPCODE_ERR, cpu -> pc);
            break;

        case R_TYPE: // arithmetic types
            uint8_t funct7 = ((ins >> 25) & 0x7F); // 31 - 25
            uint8_t funct3 = ((ins >> 12) & 0x07); // bits 14 - 12
            execute_r_type(cpu, funct3, funct7, ins);
            cpu -> pc += 4;
            break;

        case I_TYPE:
            funct3 = (ins >> 12) & 0x07;
            funct7 = (ins >> 25) & 0xFF;
            execute_i_type(cpu, funct3, funct7, ins);
            cpu -> pc += 4;
            break;

        case U_TYPE_NO_PC_INC: // u-type no1 rd = imm << 12
            uint32_t imm = ((ins >> 12) & 0xFFFFF);
            uint32_t rd = (ins >> 7) & 0x1F;
            cpu -> registers[rd] = ((uint32_t)imm << 12);
            cpu -> pc += 4;
            break;

        case U_TYPE_W_PC_INC: // u-type no2 rd = pc + imm << 12
            imm = (ins >> 12) & 0xFFFFF;
            rd = (ins >> 7) & 0x1F;
            cpu -> registers[rd] = cpu -> pc + ((uint32_t)imm << 12);
            cpu -> pc += 4;
            break;

        case B_TYPE:
            funct3 = (ins >> 12) & 0x07; // bits 14 - 12
            execute_b_type(cpu, funct3, ins);
            break;

        case JUMP_AND_LINK_TYPE: // J TYPE jump and link
            rd = (ins >> 7) & 0x1F;
            uint32_t twentieth_bit  = ((ins >> 31) & 0x01) << 20; // bit 31
            uint32_t ten_to_one = ((ins >> 21) & 0x3FF) << 1; // 30 - 21
            uint32_t eleventh_bit = (ins >> 20 & 0x01) << 11; // 20
            uint32_t nineteen_to_twelve = ((ins >> 12) & 0xFF) << 12; // 19 - 12

            imm = (twentieth_bit | ten_to_one | eleventh_bit | nineteen_to_twelve);

            int32_t old_pc = cpu -> pc;
            // rd = pc + 4; pc += imm
            cpu -> registers[rd] = (old_pc + 4);
            cpu -> pc += extend_n_bit_sign(imm, 21);
            break;

        case JUMP_AND_LINK_REG_TYPE: // J type jump and link REGISTER (I -typing ?)
            imm = extend_n_bit_sign(ins >> 20, 12);
            uint32_t rs1 = (ins >> 15) & 0x1F;
            funct3 = (ins >> 12) & 0x07;
            rd = (ins >> 7) & 0x1F;
            funct7 = (ins >> 25) & 0xFF;
            if (funct3 == 0x0) // rd = pc + 4 and pc += (imm + rs1)
            {
                cpu -> registers[rd] = (cpu -> pc + 4);
                cpu -> pc = (imm + cpu -> registers[rs1]) & ~0x01;
            }
            break;
            
        case LOAD_MEMORY_TYPE:
            funct3 = (ins >> 12) & 0x07;
            execute_load_memory(cpu, funct3, ins, perm);
            cpu -> pc += 4;
            break;
        
        case STORE_MEMORY_TYPE:
            funct3 = (ins >> 12) & 0x07; // bits 14 - 12
            execute_store_memory(cpu, funct3, ins, perm);
            cpu -> pc += 4;
            break;  
    }
    cpu -> registers[0] = 0; //0th register must be zero
}

/*
    @param *cpu: pointer to cpu_t structure
    @param entry: value to set 

    will set the program coutner to the passed in value
*/
void set_cpu_pc(cpu_t *cpu, uint32_t entry)
{
    cpu -> pc = entry;
}

/*
    @param *cpu: pointer to cpu_t structure

    Ths will fetch the current instruction from memory and for the number of segments within the instruction ensure
    that the memory is valid, and if it is then execute the given instruction otherwise throw a fatal error
*/
void fetch_decode_execute(cpu_t *cpu)
{
    if (!(PF_R & cpu -> memory -> segment_info.flag))
    {
        cpu_throw_fatal(NO_READ, cpu -> pc);
    }

    uint32_t fetched_ins = read_word(cpu -> memory, cpu -> pc);
    bool found = false;
    int found_at = 0;

    for (int i = 0; i < cpu -> memory -> num_segments; i++)
    {
        uint32_t starting_index = cpu -> memory -> segments[i].start;
        uint32_t end_index = cpu -> memory -> segments[i].end; 

        if (starting_index <= cpu -> pc && cpu -> pc < end_index) 
        {
            found = true;
            found_at = i;
            break;
        }
    }

    if (!found)
    {
        cpu_throw_fatal(ILLEGAL_MEM_ACCESS, cpu -> pc);
    }
    else
    {
        uint32_t perm = cpu -> memory -> segments[found_at].flag;
        if (PF_X & perm)
        {
            emulate_instruction(cpu, fetched_ins, perm);
        }
        else
        {
            cpu_throw_fatal(NO_EXEC, cpu -> pc);
        }
    }

}

/*
    @param cpu: cpu_t struct 
    @param ins: RISC-V 32bit instruction
    @param old reg: register value before instruction execution
    @param old_pc: old program counter value before execution
    the tracing function for the cpu will populate the cpu_info_t struct with the 
    relevant data for debugging / general purpose to see which info actually gets stored
    visit the definition of the cpu_iinfo_t struct
*/
void trace_cpu(cpu_t *cpu, uint32_t ins, uint32_t *old_reg, uint32_t old_pc)
{
    cpu_info_t log;
    char buffer[256]; 
    log.opcode = (ins & 0x0FF);
    log.new_pc = (long) (cpu -> pc);
    log.old_pc = (long)old_pc;

    for (int i = 0; i < X_LEN; i++)
    {
        log.old_reg_val = (long)old_reg[i];
        log.new_reg_val = (long)cpu -> registers[i];
        log.register_affected = (long)i;
        if (old_reg[i] != cpu -> registers[i])
        {
            break;
        }
    }
    cpu -> data = log;
}

void cpu_loop(cpu_t *cpu)
{
    bool running = true;
    while (running)
    {
        fetch_decode_execute(cpu);
    }
}

void cpu_loop_for_testing(cpu_t *cpu, int num_cycles, char* func_name)
{
    for (int i = 0; i < num_cycles; i++)
    {
        uint32_t ins = read_word(cpu -> memory, cpu -> pc);
        uint32_t opcode = ins & 0x0FF;

        uint32_t *old_registers = deep_copy(cpu -> registers, X_LEN);
        uint32_t old_pc = cpu -> pc;

        fetch_decode_execute(cpu);
        trace_cpu(cpu, ins, old_registers, old_pc);
        putchar('\n');
        printf("DEBUG LOG FOR [ %s ] iter: %d \n", func_name, i);
        if (cpu -> data.old_reg_val == cpu -> data.new_reg_val)
        {
            printf("No register affected\n");
        }
        else
        {
            printf("register x%ld: %ld -> %ld \n", 
            cpu -> data.register_affected, cpu -> data.old_reg_val, cpu -> data.new_reg_val);
        }
        printf("Old PC %ld -> New PC %ld\n", cpu -> data.old_pc, cpu -> data.new_pc);
        printf("Opcode (hex): %X\n", cpu -> data.opcode);
        putchar('\n');
    }
}





