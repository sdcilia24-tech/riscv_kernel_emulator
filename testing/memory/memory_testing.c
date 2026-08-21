#include "unity_internals.h"
#include <stdint.h>
#include <unity.h>
#include "memory/memory.h"
#include "cpu/cpu.h"
void setUp(void)
{

}
void tearDown(void)
{


}
void test_fde_assem_ins(void)
{
    // asm bin 0000 9300f000 13015000 b3011100 67800000 OLD
    uint32_t ins1 = 0x00f00093;
    uint32_t ins2 = 0x00500113;
    uint32_t ins3 = 0x001101b3;
    uint32_t ins4 = 0x00008067;

    cpu_t cpu = {0};
    mem_t memory = {0};
    cpu.memory = &memory;

    write_word(cpu.memory, 0x0, ins1);
    write_word(cpu.memory, 0x4, ins2);
    write_word(cpu.memory, 0x8, ins3);
    write_word(cpu.memory, 0x0C, ins4);

    cpu_loop_for_testing(&cpu, 4, "test_fde_assem_ins");

    TEST_ASSERT_EQUAL_UINT32(15, cpu.registers[1]);
    TEST_ASSERT_EQUAL_UINT32(5, cpu.registers[2]);
    TEST_ASSERT_EQUAL_UINT32(20, cpu.registers[3]);
}

void test_fde_assem_again(void)
{
    uint32_t ins[] = {
        0x00f00093,
        0x00500113,
        0x001101b3,
        0x00209663,
        0xfff00793,
        0x0407c213,
        0x00008067
    };
    cpu_t cpu = {0};
    mem_t memory = {0};
    cpu.memory = &memory;
    for (int i = 0; i <= 6; i++)
    {
        write_word(cpu.memory, i * 4, ins[i]);
    }
    cpu_loop_for_testing(&cpu, 4, "test_fde_assem_again");

    TEST_ASSERT_EQUAL_UINT32(24, cpu.pc);

}

void test_write_word(void)
{
    mem_t memory = {0};
    write_word(&memory, 0, 0x00F00093);
    uint32_t retrieve = read_word(&memory, 0);

    TEST_ASSERT_EQUAL_UINT8(0x93, memory.map[0]);
    TEST_ASSERT_EQUAL_UINT8(0x00, memory.map[1]);
    TEST_ASSERT_EQUAL_UINT8(0xF0, memory.map[2]);
    TEST_ASSERT_EQUAL_UINT8(0x00, memory.map[3]);

    TEST_ASSERT_EQUAL_UINT32(
        0x00F00093,
        read_word(&memory, 0)
    );
}

void test_write_half(void)
{
    mem_t memory = {0};
    write_half(&memory, 0, 0x0FFFF);
    uint32_t retrieve = read_half(&memory, 0);
    
    TEST_ASSERT_EQUAL_UINT32(0x0FFFF, retrieve);
}

void test_write_byte(void)
{
    mem_t memory = {0};
    write_byte(&memory, 0, 0xFF);

    uint32_t retrieve = read_byte(&memory, 0);

    TEST_ASSERT_EQUAL_UINT32(0xFF, retrieve);
    

}

void test_store_and_load_byte(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;

    // Put the address in x1
    cpu.registers[1] = 0x100;

    // Put the value we want to store in x2
    cpu.registers[2] = 0xAB;

    // SB x2, 0(x1)
    uint32_t store_instruction =
        (0x0 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (0x0 << 7) |
        STORE_MEMORY_TYPE;

    emulate_instruction(&cpu, store_instruction);

    TEST_ASSERT_EQUAL_UINT8(0xAB, memory.map[0x100]);

    // LB x3, 0(x1)
    uint32_t load_instruction =
        (0x0 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (3 << 7) |
        LOAD_MEMORY_TYPE;

    emulate_instruction(&cpu, load_instruction);

    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFAB, cpu.registers[3]);
}

void test_store_and_load_halfword(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;

    // x1 = address
    cpu.registers[1] = 0x100;

    // x2 = value to store
    cpu.registers[2] = 0x1234;

    // SH x2, 0(x1)
    uint32_t store_instruction =
        (0x0 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (0x0 << 7) |
        STORE_MEMORY_TYPE;

    emulate_instruction(&cpu, store_instruction);

    // Make sure the halfword was actually stored
    TEST_ASSERT_EQUAL_UINT8(0x34, memory.map[0x100]);
    TEST_ASSERT_EQUAL_UINT8(0x12, memory.map[0x101]);

    // LH x3, 0(x1)
    uint32_t load_instruction =
        (0x0 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (3 << 7) |
        LOAD_MEMORY_TYPE;

    emulate_instruction(&cpu, load_instruction);

    TEST_ASSERT_EQUAL_UINT32(0x1234, cpu.registers[3]);
}

void test_store_and_load_byte_unsigned(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;

    // x1 = address
    cpu.registers[1] = 0x100;

    // x2 = value to store
    cpu.registers[2] = 0xAB;

    // SB x2, 0(x1)
    uint32_t store_instruction =
        (0x0 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (0x0 << 7) |
        STORE_MEMORY_TYPE;

    emulate_instruction(&cpu, store_instruction);

    TEST_ASSERT_EQUAL_UINT8(0xAB, memory.map[0x100]);

    // LBU x3, 0(x1)
    uint32_t load_instruction =
        (0x0 << 20) |
        (1 << 15) |
        (0x4 << 12) |
        (3 << 7) |
        LOAD_MEMORY_TYPE;

    emulate_instruction(&cpu, load_instruction);

    // Unsigned byte should be zero-extended
    TEST_ASSERT_EQUAL_UINT32(0x000000AB, cpu.registers[3]);
}


void test_fde_jal(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};
    cpu.memory = &memory;

    // recieving register

    cpu.pc = 0x200;

    // opcode = j_type

    uint32_t offset = 8;
    uint32_t instruct = (((offset >> 1) & 0x3FF) << 21) | 1 << 7 | JUMP_AND_LINK_TYPE;
    write_word(&memory, 0x200, instruct);
    cpu_loop_for_testing(&cpu, 1, "test_fde_jal");
    TEST_ASSERT_EQUAL_INT(0x200 + 4, cpu.registers[1]);
    TEST_ASSERT_EQUAL_INT(0x200 + offset, cpu.pc);
    
}

void test_store_and_load_halfword_unsigned(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;

    // x1 = address
    cpu.registers[1] = 0x100;

    // x2 = value to store
    cpu.registers[2] = 0xABCD;

    // SH x2, 0(x1)
    uint32_t store_instruction =
        (0x0 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x1 << 12) |
        (0x0 << 7) |
        STORE_MEMORY_TYPE;

    emulate_instruction(&cpu, store_instruction);

    // Verify little-endian representation
    TEST_ASSERT_EQUAL_UINT8(0xCD, memory.map[0x100]);
    TEST_ASSERT_EQUAL_UINT8(0xAB, memory.map[0x101]);

    // LHU x3, 0(x1)
    uint32_t load_instruction =
        (0x0 << 20) |
        (1 << 15) |
        (0x5 << 12) |
        (3 << 7) |
        LOAD_MEMORY_TYPE;

    emulate_instruction(&cpu, load_instruction);

    // Unsigned halfword should be zero-extended
    TEST_ASSERT_EQUAL_UINT32(0x0000ABCD, cpu.registers[3]);
}

void test_store_and_load_word(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;

    // x1 = address
    cpu.registers[1] = 0x100;

    // x2 = value to store
    cpu.registers[2] = 0x12345678;

    // SW x2, 0(x1)
    uint32_t store_instruction =
        (0x0 << 25) |
        (2 << 20) |
        (1 << 15) |
        (0x2 << 12) |
        (0x0 << 7) |
        STORE_MEMORY_TYPE;

    emulate_instruction(&cpu, store_instruction);

    // Verify little-endian representation
    TEST_ASSERT_EQUAL_UINT8(0x78, memory.map[0x100]);
    TEST_ASSERT_EQUAL_UINT8(0x56, memory.map[0x101]);
    TEST_ASSERT_EQUAL_UINT8(0x34, memory.map[0x102]);
    TEST_ASSERT_EQUAL_UINT8(0x12, memory.map[0x103]);

    // LW x3, 0(x1)
    uint32_t load_instruction =
        (0x0 << 20) |
        (1 << 15) |
        (0x2 << 12) |
        (3 << 7) |
        LOAD_MEMORY_TYPE;

    emulate_instruction(&cpu, load_instruction);

    TEST_ASSERT_EQUAL_UINT32(0x12345678, cpu.registers[3]);
}


void test_fde_executes_instruction(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x100;

    // ADDI x1, x0, 42
    uint32_t instruction =
        (42 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (1 << 7) |
        I_TYPE;

    write_word(&memory, 0x100, instruction);

    fetch_decode_execute(&cpu);

    TEST_ASSERT_EQUAL_UINT32(42, cpu.registers[1]);
}

void test_fde_advances_pc(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x100;

    // ADDI x1, x0, 42
    uint32_t instruction =
        (42 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (1 << 7) |
        I_TYPE;

    write_word(&memory, 0x100, instruction);

    fetch_decode_execute(&cpu);

    TEST_ASSERT_EQUAL_UINT32(0x104, cpu.pc);
}

void test_fde_executes_multiple_instructions(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x100;

    // ADDI x1, x0, 10
    uint32_t instruction1 =
        (10 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (1 << 7) |
        I_TYPE;

    // ADDI x2, x1, 20
    uint32_t instruction2 =
        (20 << 20) |
        (1 << 15) |
        (0x0 << 12) |
        (2 << 7) |
        I_TYPE;

    write_word(&memory, 0x100, instruction1);
    write_word(&memory, 0x104, instruction2);

    cpu_loop_for_testing(&cpu, 2, "test_fde_executes_multiple_instructions");

    TEST_ASSERT_EQUAL_UINT32(10, cpu.registers[1]);
    TEST_ASSERT_EQUAL_UINT32(30, cpu.registers[2]);
    TEST_ASSERT_EQUAL_UINT32(0x108, cpu.pc);
}

void test_fde_fetches_from_current_pc(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x200;

    // ADDI x5, x0, 55
    uint32_t instruction =
        (55 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (5 << 7) |
        I_TYPE;

    write_word(&memory, 0x200, instruction);

    fetch_decode_execute(&cpu);

    TEST_ASSERT_EQUAL_UINT32(55, cpu.registers[5]);
    TEST_ASSERT_EQUAL_UINT32(0x204, cpu.pc);
}

void test_fde_branch_not_taken(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x100;

    // x1 != x2, so BEQ should NOT branch
    cpu.registers[1] = 10;
    cpu.registers[2] = 20;

    /*
        BEQ x1, x2, +8

        Since x1 != x2:
        0x100 -> 0x104
    */
    uint32_t branch =
        (2 << 20) |      // rs2 = x2
        (1 << 15) |      // rs1 = x1
        (4 << 8)  |      // imm[4:1] = 0100 (+8)
        B_TYPE;

    // This instruction SHOULD execute
    // ADDI x3, x0, 111
    uint32_t executed =
        (111 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (3 << 7) |
        I_TYPE;

    // This instruction should NOT execute
    // ADDI x4, x0, 222
    uint32_t skipped =
        (222 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (4 << 7) |
        I_TYPE;

    write_word(&memory, 0x100, branch);
    write_word(&memory, 0x104, executed);
    write_word(&memory, 0x108, skipped);

    cpu_loop_for_testing(&cpu, 2, "test_fde_branch_not_taken");

    TEST_ASSERT_EQUAL_UINT32(111, cpu.registers[3]);
    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[4]);

    // 0x100 -> 0x104 -> 0x108
    TEST_ASSERT_EQUAL_UINT32(0x108, cpu.pc);
}

void test_fde_branch_skips_instruction(void)
{
    cpu_t cpu = {0};
    mem_t memory = {0};

    cpu.memory = &memory;
    cpu.pc = 0x100;

    cpu.registers[1] = 10;
    cpu.registers[2] = 10;

    /*
       BEQ x1, x2, +8

       0x100 → 0x108
    */
    uint32_t branch =
        (2 << 20) |
        (1 << 15) |
        (4 << 8)  |
        B_TYPE;

    // ADDI x3, x0, 111
    uint32_t skipped =
        (111 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (3 << 7) |
        I_TYPE;

    // ADDI x4, x0, 222
    uint32_t target =
        (222 << 20) |
        (0 << 15) |
        (0x0 << 12) |
        (4 << 7) |
        I_TYPE;

    write_word(&memory, 0x100, branch);
    write_word(&memory, 0x104, skipped);
    write_word(&memory, 0x108, target);

    cpu_loop_for_testing(&cpu, 2, "test_fde_branch_skips_instruction");

    TEST_ASSERT_EQUAL_UINT32(0, cpu.registers[3]);
    TEST_ASSERT_EQUAL_UINT32(222, cpu.registers[4]);
    TEST_ASSERT_EQUAL_UINT32(0x10C, cpu.pc);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_fde_branch_skips_instruction);
    RUN_TEST(test_fde_fetches_from_current_pc);
    RUN_TEST(test_fde_executes_instruction);
    RUN_TEST(test_fde_executes_multiple_instructions);
    RUN_TEST(test_fde_branch_not_taken);
    RUN_TEST(test_fde_jal);
    RUN_TEST(test_fde_assem_ins);
    RUN_TEST(test_fde_assem_again);

    RUN_TEST(test_write_word);
    RUN_TEST(test_write_half);
    RUN_TEST(test_write_byte);

    
    RUN_TEST(test_store_and_load_byte);
    RUN_TEST(test_store_and_load_halfword);
    RUN_TEST(test_store_and_load_word);
    RUN_TEST(test_store_and_load_halfword_unsigned);
    RUN_TEST(test_store_and_load_byte_unsigned);
    return UNITY_END();
}