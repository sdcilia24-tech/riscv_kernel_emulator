#include "cpu/cpu.h"
#include "unity_internals.h"
#include <stdint.h>
#include <unity.h>

void setUp(void)
{

}

void tearDown(void)
{

}

uint32_t encode_j_type(int32_t imm, uint8_t rd)
{
    uint32_t instruction = 0;

    uint32_t imm20 = (imm >> 20) & 0x1;
    uint32_t imm10_1 = (imm >> 1) & 0x3FF;
    uint32_t imm11 = (imm >> 11) & 0x1;
    uint32_t imm19_12 = (imm >> 12) & 0xFF;

    instruction |= (imm20 << 31);
    instruction |= (imm10_1 << 21);
    instruction |= (imm11 << 20);
    instruction |= (imm19_12 << 12);
    instruction |= (rd << 7);
    instruction |= 0x6F;

    return instruction;
}

void test_jal_forward(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    uint32_t instruction =
        encode_j_type(0x100, 1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1100, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x1004, cpu.registers[1]);
}

void test_jal_backward(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x2000;

    uint32_t instruction =
        encode_j_type(-0x100, 5);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1F00, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x2004, cpu.registers[5]);
}

void test_jal_smallest_positive_offset(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    uint32_t instruction =
        encode_j_type(2, 3);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1002, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x1004, cpu.registers[3]);
}

void test_jal_zero_offset(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x5000;

    uint32_t instruction =
        encode_j_type(0, 7);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x5000, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x5004, cpu.registers[7]);
}

void test_jal_x0(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    uint32_t instruction =
        encode_j_type(0x100, 0);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1100, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0, cpu.registers[0]);
}

uint32_t encode_jalr(int32_t imm, uint8_t rs1, uint8_t rd)
{
    uint32_t instruction = 0;

    instruction |= ((uint32_t)imm & 0xFFF) << 20;
    instruction |= rs1 << 15;
    instruction |= 0x0 << 12;
    instruction |= rd << 7;
    instruction |= 0x67;

    return instruction;
}

void test_jalr_forward(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;
    cpu.registers[2] = 0x2000;

    uint32_t instruction =
        encode_jalr(0x20, 2, 1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x2020, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x1004, cpu.registers[1]);
}

void test_jalr_negative_offset(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x3000;
    cpu.registers[2] = 0x2000;

    uint32_t instruction =
        encode_jalr(-0x20, 2, 1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1FE0, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x3004, cpu.registers[1]);
}

void test_jalr_clears_bit_zero(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;
    cpu.registers[2] = 0x2001;

    uint32_t instruction =
        encode_jalr(0, 2, 1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x2000, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0x1004, cpu.registers[1]);
}

void test_jalr_return(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x5000;
    cpu.registers[1] = 0x1234;

    uint32_t instruction =
        encode_jalr(0, 1, 0);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1234, cpu.pc);
    TEST_ASSERT_EQUAL_HEX32(0, cpu.registers[0]);
}

void test_lui_simple(void)
{
    cpu_t cpu = {0};

    uint32_t instruction =
        (0x12345 << 12) |
        (5 << 7) |
        0x37;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x12345000, cpu.registers[5]);
}

void test_lui_zero(void)
{
    cpu_t cpu = {0};

    uint32_t instruction =
        (0x00000 << 12) |
        (3 << 7) |
        0x37;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x00000000, cpu.registers[3]);
}

void test_lui_all_ones(void)
{
    cpu_t cpu = {0};

    uint32_t instruction =
        (0xFFFFF << 12) |
        (8 << 7) |
        0x37;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0xFFFFF000, cpu.registers[8]);
}

void test_auipc_simple(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    uint32_t instruction =
        (0x2 << 12) |
        (6 << 7) |
        0x17;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x3000, cpu.registers[6]);
}

void test_auipc_zero_immediate(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x2000;

    uint32_t instruction =
        (0x0 << 12) |
        (4 << 7) |
        0x17;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x2000, cpu.registers[4]);
}

void test_auipc_large_immediate(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x100;

    uint32_t instruction =
        (0xABCDE << 12) |
        (7 << 7) |
        0x17;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0xABCDE100, cpu.registers[7]);
}

void test_lui_write_x0(void)
{
    cpu_t cpu = {0};

    uint32_t instruction =
        (0x12345 << 12) |
        (0 << 7) |
        0x37;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[0]);
}

void test_auipc_write_x0(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    uint32_t instruction =
        (0x1 << 12) |
        (0 << 7) |
        0x17;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[0]);
}

void test_add_method(void) 
{
    cpu_t cpu = {0};

    cpu.registers[1] = 10;
    cpu.registers[2] = 20;

    uint32_t instruction =
        (0x00 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (3 << 7) |
        0x33;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT_MESSAGE(30, cpu.registers[3], "testing addition");
}

void test_subtract_method(void)
{
    
    cpu_t cpu = {0};
    cpu.registers[3] = 5;
    cpu.registers[2] = 4;
    
    uint32_t instruction =
        (0x20 << 25) |  // funct7
        (2 << 20)    |  // rs2
        (3 << 15)    |  // rs1
        (0x0 << 12)  |  // funct3
        (1 << 7)     |  // rd
        0x33;          // opcode

    emulate_instruction(&cpu, instruction);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, cpu.registers[1], " 5 - 4 = 1");

}

void test_xor(void) 
{
    cpu_t cpu = {0};
    cpu.registers[1] = 1;
    cpu.registers[2] = 0;

    uint32_t instruction =
    (0x20 << 25) |  // funct7
    (2 << 20)    |  // rs2
    (1 << 15)    |  // rs1
    (0x0 << 12)  |  // funct3
    (1 << 7)     |  // rd
    0x33;          // opcode
    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT_MESSAGE(1, cpu.registers[1], "1 XOR 0 = 1");
}


// Helper for R-type instructions
uint32_t encode_r_type(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd)
{
    return (funct7 << 25) |
           (rs2 << 20) |
           (rs1 << 15) |
           (funct3 << 12) |
           (rd << 7) |
           0x33;
}


// OR x3, x1, x2
void test_or(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0b1010;
    cpu.registers[2] = 0b0101;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x6,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0b1111, cpu.registers[3]);
}


// AND x3, x1, x2
void test_and(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0b1100;
    cpu.registers[2] = 0b1010;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x7,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0b1000, cpu.registers[3]);
}


// SLL x3, x1, x2
void test_sll(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 5;
    cpu.registers[2] = 2;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x1,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(20, cpu.registers[3]);
}


// SRL x3, x1, x2
void test_srl(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x80000000;
    cpu.registers[2] = 4;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x5,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0x08000000, cpu.registers[3]);
}


// SRA x3, x1, x2
void test_sra(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x80000000;
    cpu.registers[2] = 4;

    uint32_t instruction = encode_r_type(
        0x20,
        2,
        1,
        0x5,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0xF8000000, cpu.registers[3]);
}


// SLT signed true case
void test_slt_true(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = -5;
    cpu.registers[2] = 3;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x2,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(1, cpu.registers[3]);
}


// SLT signed false case
void test_slt_false(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 5;
    cpu.registers[2] = 3;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x2,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[3]);
}


// SLTU unsigned comparison
void test_sltu(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0xFFFFFFFF;
    cpu.registers[2] = 1;

    uint32_t instruction = encode_r_type(
        0x00,
        2,
        1,
        0x3,
        3
    );

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[3]);
}

void test_addi(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 10;

    uint32_t instruction =
        (5 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(15, cpu.registers[2]);
}

void test_addi_negative_immediate(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 10;

    // imm = -3 = 0xFFD (12-bit)
    uint32_t instruction =
        (0xFFD << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(7, cpu.registers[2]);
}

void test_xori(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0xAA;

    uint32_t instruction =
        (0x55 << 20) |
        (1 << 15) |
        (0x4 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0xFF, cpu.registers[2]);
}

void test_ori(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0xA0;

    uint32_t instruction =
        (0x0F << 20) |
        (1 << 15) |
        (0x6 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0xAF, cpu.registers[2]);
}

void test_andi(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0xAF;

    uint32_t instruction =
        (0x0F << 20) |
        (1 << 15) |
        (0x7 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x0F, cpu.registers[2]);
}

void test_slli(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 1;

    uint32_t instruction =
        (3 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(8, cpu.registers[2]);
}

void test_srli(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 32;

    uint32_t instruction =
        (2 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(8, cpu.registers[2]);
}

void test_srai(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = (uint32_t)-8;

    uint32_t instruction =
        (0x20 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(-2, (int32_t)cpu.registers[2]);
}
void test_slli_shift_by_zero(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x12345678;

    uint32_t instruction =
        (0 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x12345678, cpu.registers[2]);
}

void test_srli_shift_by_zero(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x87654321;

    uint32_t instruction =
        (0 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x87654321, cpu.registers[2]);
}

void test_srai_shift_by_zero(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = (uint32_t)-12345;

    uint32_t instruction =
        (0x20 << 25) |
        (0 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(-12345, (int32_t)cpu.registers[2]);
}

void test_slli_shift_by_31(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 1;

    uint32_t instruction =
        (31 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x80000000, cpu.registers[2]);
}

void test_srli_shift_by_31(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x80000000;

    uint32_t instruction =
        (31 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(1, cpu.registers[2]);
}

void test_srai_shift_by_31(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0x80000000;

    uint32_t instruction =
        (0x20 << 25) |
        (31 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(-1, (int32_t)cpu.registers[2]);
}

void test_srai_negative_one(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = (uint32_t)-1;

    uint32_t instruction =
        (0x20 << 25) |
        (31 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(-1, (int32_t)cpu.registers[2]);
}

void test_addi_into_x0(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 50;

    uint32_t instruction =
        (10 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (0 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[0]);
}

void test_slti_true(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 5;

    uint32_t instruction =
        (10 << 20) |
        (1 << 15) |
        (0x2 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(1, cpu.registers[2]);
}

void test_slti_false(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 20;

    uint32_t instruction =
        (10 << 20) |
        (1 << 15) |
        (0x2 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[2]);
}
void test_addi_negative_boundary(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0;

    // imm = -2048 (smallest 12-bit signed value)
    uint32_t instruction =
        (0x800 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(-2048, (int32_t)cpu.registers[2]);
}
void test_sltiu(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0xFFFFFFFF;

    uint32_t instruction =
        (1 << 20) |
        (1 << 15) |
        (0x3 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[2]);
}

void test_addi_positive_boundary(void)
{
    cpu_t cpu = {0};

    cpu.registers[1] = 0;

    // imm = 2047 (largest 12-bit signed value)
    uint32_t instruction =
        (0x7FF << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (2 << 7) |
        0x13;

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_INT32(2047, (int32_t)cpu.registers[2]);
}

uint32_t encode_b_type(
    int32_t imm,
    uint8_t rs2,
    uint8_t rs1,
    uint8_t funct3
)
{
    uint32_t instruction = 0;

    uint32_t imm12 = (imm >> 12) & 0x1;
    uint32_t imm11 = (imm >> 11) & 0x1;
    uint32_t imm10_5 = (imm >> 5) & 0x3F;
    uint32_t imm4_1 = (imm >> 1) & 0xF;

    instruction |= (imm12 << 31);
    instruction |= (imm10_5 << 25);
    instruction |= (rs2 << 20);
    instruction |= (rs1 << 15);
    instruction |= (funct3 << 12);
    instruction |= (imm4_1 << 8);
    instruction |= (imm11 << 7);
    instruction |= 0x63;

    return instruction;
}

void test_beq_taken(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    cpu.registers[1] = 10;
    cpu.registers[2] = 10;

    uint32_t instruction =
        encode_b_type(8, 2, 1, 0x0);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1008, cpu.pc);
}


void test_beq_not_taken(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x1000;

    cpu.registers[1] = 10;
    cpu.registers[2] = 20;

    uint32_t instruction =
        encode_b_type(8, 2, 1, 0x0);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x1004, cpu.pc);
}

void test_blt_signed_true(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x3000;

    cpu.registers[1] = (uint32_t)-5;
    cpu.registers[2] = 3;

    uint32_t instruction =
        encode_b_type(4, 2, 1, 0x4);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x3004, cpu.pc);
}


void test_blt_signed_false(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x3000;

    cpu.registers[1] = 5;
    cpu.registers[2] = 3;

    uint32_t instruction =
        encode_b_type(4, 2, 1, 0x4);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x3004, cpu.pc);
}

void test_bltu_true(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x5000;

    cpu.registers[1] = 1;
    cpu.registers[2] = 5;

    uint32_t instruction =
        encode_b_type(4, 2, 1, 0x6);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x5004, cpu.pc);
}


void test_bltu_negative_value_is_large(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x5000;

    cpu.registers[1] = 0xFFFFFFFF;
    cpu.registers[2] = 1;

    uint32_t instruction =
        encode_b_type(4, 2, 1, 0x6);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x5004, cpu.pc);
}

void test_bne_taken(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x2000;

    cpu.registers[1] = 5;
    cpu.registers[2] = 6;

    uint32_t instruction =
        encode_b_type(12, 2, 1, 0x1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x200C, cpu.pc);
}


void test_bne_not_taken(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x2000;

    cpu.registers[1] = 5;
    cpu.registers[2] = 5;

    uint32_t instruction =
        encode_b_type(12, 2, 1, 0x1);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x2004, cpu.pc);
}

void test_bge_signed_true(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x4000;

    cpu.registers[1] = 5;
    cpu.registers[2] = 3;

    uint32_t instruction =
        encode_b_type(16, 2, 1, 0x5);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x4010, cpu.pc);
}


void test_bge_signed_false(void)
{
    cpu_t cpu = {0};

    cpu.pc = 0x4000;

    cpu.registers[1] = (uint32_t)-10;
    cpu.registers[2] = 3;

    uint32_t instruction =
        encode_b_type(16, 2, 1, 0x5);

    emulate_instruction(&cpu, instruction);

    TEST_ASSERT_EQUAL_HEX32(0x4004, cpu.pc);
}




int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_add_method);
    RUN_TEST(test_subtract_method);
    RUN_TEST(test_xor);
    RUN_TEST(test_or);
    RUN_TEST(test_and);
    RUN_TEST(test_sll);
    RUN_TEST(test_srl);
    RUN_TEST(test_sra);
    RUN_TEST(test_slt_true);
    RUN_TEST(test_slt_false);
    RUN_TEST(test_sltu);

    RUN_TEST(test_addi);
    RUN_TEST(test_addi_negative_immediate);
    RUN_TEST(test_xori);
    RUN_TEST(test_ori);
    RUN_TEST(test_andi);
    RUN_TEST(test_slli);
    RUN_TEST(test_srli);
    RUN_TEST(test_srai);
    RUN_TEST(test_slti_true);
    RUN_TEST(test_slti_false);
    RUN_TEST(test_sltiu);

    RUN_TEST(test_slli_shift_by_zero);
    RUN_TEST(test_srli_shift_by_zero);
    RUN_TEST(test_srai_shift_by_zero);

    RUN_TEST(test_slli_shift_by_31);
    RUN_TEST(test_srli_shift_by_31);
    RUN_TEST(test_srai_shift_by_31);

    RUN_TEST(test_srai_negative_one);

    RUN_TEST(test_addi_into_x0);
    RUN_TEST(test_addi_negative_boundary);
    RUN_TEST(test_addi_positive_boundary);


    RUN_TEST(test_lui_simple);
    RUN_TEST(test_lui_zero);
    RUN_TEST(test_lui_all_ones);

    RUN_TEST(test_auipc_simple);
    RUN_TEST(test_auipc_zero_immediate);
    RUN_TEST(test_auipc_large_immediate);

    RUN_TEST(test_lui_write_x0);
    RUN_TEST(test_auipc_write_x0);


    RUN_TEST(test_beq_taken);
    RUN_TEST(test_beq_not_taken);

    RUN_TEST(test_bne_taken);
    RUN_TEST(test_bne_not_taken);

    RUN_TEST(test_blt_signed_true);
    RUN_TEST(test_blt_signed_false);

    RUN_TEST(test_bge_signed_true);
    RUN_TEST(test_bge_signed_false);

    RUN_TEST(test_bltu_true);

    RUN_TEST(test_jal_forward);
    RUN_TEST(test_jal_backward);
    RUN_TEST(test_jal_smallest_positive_offset);
    RUN_TEST(test_jal_zero_offset);
    RUN_TEST(test_jal_x0);

    RUN_TEST(test_jalr_forward);
    RUN_TEST(test_jalr_negative_offset);
    RUN_TEST(test_jalr_clears_bit_zero);
RUN_TEST(test_jalr_return);
    RUN_TEST(test_bltu_negative_value_is_large);
    return UNITY_END();
}