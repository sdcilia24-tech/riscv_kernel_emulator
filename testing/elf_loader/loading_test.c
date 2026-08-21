#include "unity_internals.h"
#include <stdbool.h>
#include <stdint.h>
#include <unity.h>
#include "elf_loading/elf_loader.h"


void setUp()
{

}
void tearDown()
{

}

void test_header_populate(void)
{
    file_info_t info = flatten_file("C:/Users/sdcil/CProjects/riscv_kernel/src/assembly_test/test.elf");
    Elf32_Ehdr header = populate_header(&info);
    program_header p_h = populate_pgrm_header(&header, &info, 1);
    mem_t memory;
    TEST_ASSERT_EQUAL(header_check(&header), true);
    TEST_ASSERT_EQUAL(is_supported(&header), true);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_header_populate);
    return UNITY_END();
}