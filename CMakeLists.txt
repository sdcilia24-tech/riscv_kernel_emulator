cmake_minimum_required(VERSION 3.15)
project(riscv_kernel LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

# ==========================================
# 1. ISOLATE CORE APPLICATION LOGIC
# ==========================================
add_library(app_logic src/cpu/cpu.c)
add_library(app_memory src/memory/memory.c)
add_library(loader src/elf_loading/elf_loader.c)
add_library(dsm src/disassembler/disassembler.c)
target_include_directories(app_logic PUBLIC src)
target_include_directories(dsm PUBLIC src)
target_include_directories(loader PUBLIC src)
target_include_directories(app_memory PUBLIC src)
target_link_libraries(app_logic PRIVATE app_memory)
target_link_libraries(app_logic PRIVATE dsm)

# ==========================================
# 2. MAIN EXECUTABLE PROGRAM
# ==========================================
add_executable(riscv riscv_main.c)
target_link_libraries(riscv PRIVATE app_logic)
target_link_libraries(riscv PRIVATE app_memory)
target_link_libraries(riscv PRIVATE loader)
target_link_libraries(riscv PRIVATE dsm)


# ==========================================
# 3. CPU UNIT TESTS
# ==========================================

add_executable(cpu_tests
    testing/cpu/cpu_testing.c
    testing/unity.c
)

target_include_directories(cpu_tests PRIVATE
    testing
    src
)

target_link_libraries(cpu_tests PRIVATE
    app_logic
    app_memory
)


# ==========================================
# 4. MEMORY UNIT TESTS
# ==========================================

add_executable(memory_tests
    testing/memory/memory_testing.c
    testing/unity.c
)

target_include_directories(memory_tests PRIVATE
    testing
    src
)

target_link_libraries(memory_tests PRIVATE
    loader
    app_memory
    app_logic
)


# ==========================================
# 4. ELFLOADER UNIT TESTS
# ==========================================

add_executable(loader_test
    testing/elf_loader/loading_test.c
    testing/unity.c
)

target_include_directories(loader_test PRIVATE
    testing
    src
)

target_link_libraries(loader_test PRIVATE
    loader
    app_memory
    app_logic
)

# ==========================================
# 5. REGISTER TESTS
# ==========================================

add_test(NAME cpuUnitTests COMMAND cpu_tests)
add_test(NAME memoryUnitTests COMMAND memory_tests)
add_test(NAME ElfLoaderTests COMMAND loader_test)