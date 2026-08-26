# CMake generated Testfile for 
# Source directory: C:/Users/sdcil/CProjects/riscv_kernel
# Build directory: C:/Users/sdcil/CProjects/riscv_kernel/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cpuUnitTests "C:/Users/sdcil/CProjects/riscv_kernel/build/cpu_tests.exe")
set_tests_properties(cpuUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;98;add_test;C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;0;")
add_test(memoryUnitTests "C:/Users/sdcil/CProjects/riscv_kernel/build/memory_tests.exe")
set_tests_properties(memoryUnitTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;99;add_test;C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;0;")
add_test(ElfLoaderTests "C:/Users/sdcil/CProjects/riscv_kernel/build/loader_test.exe")
set_tests_properties(ElfLoaderTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;100;add_test;C:/Users/sdcil/CProjects/riscv_kernel/CMakeLists.txt;0;")
