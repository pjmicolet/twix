# ============================================================================
# Component Libraries
# ============================================================================

# Assembler utilities library
add_library(assembler_utils 
    components/assembler/utils/asm_utils.hpp
    components/assembler/utils/asm_utils.cpp
)

target_include_directories(assembler_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

# MOS 6502 assembler/disassembler library
add_library(mos6502
    components/assembler/mos6502/mos6502.hpp
    components/assembler/mos6502/mos6502.cpp
)

target_include_directories(mos6502 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/components
)

target_link_libraries(mos6502 PUBLIC
    assembler_utils
)

# MOS 6502 CPU core library
add_library(mos6502core
    components/cores/mos6502/instructions.hpp
    components/cores/mos6502/instructions.cpp
)

target_include_directories(mos6502core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components/cores>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(mos6502core INTERFACE SYSTEM
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/components/cores>
)

# ============================================================================
# Test Framework
# ============================================================================

add_library(test_framework INTERFACE
    tests/framework/testing.hpp
    tests/framework/result_format.hpp
    tests/framework/assertions.hpp
)

target_include_directories(test_framework INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# ============================================================================
# Test Executables
# ============================================================================

# Assembler utils tests
add_executable(Test 
    tests/components/assembler/utils/asm_utils_test.cpp
)

target_link_libraries(Test PUBLIC
    assembler_utils 
    test_framework
)

# MOS 6502 assembler/disassembler tests
add_executable(mos6502_test_suite 
    tests/components/assembler/mos6502/test_main.cpp
    tests/components/assembler/mos6502/assembler.hpp
    tests/components/assembler/mos6502/disassembler.hpp
)

target_link_libraries(mos6502_test_suite PUBLIC
    test_framework
    mos6502
)

# MOS 6502 core tests
add_executable(mos6502_core_test_suite 
    tests/components/cores/mos6502/test_main.cpp
    tests/components/cores/mos6502/mos6502_test.hpp
)

target_link_libraries(mos6502_core_test_suite PUBLIC
    test_framework
    mos6502core
    mos6502
)
