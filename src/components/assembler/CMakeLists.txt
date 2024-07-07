add_library(assembler_utils 
   utils/asm_utils.hpp
   utils/asm_utils.cpp
)

add_library(mos6502
    mos6502/mos6502.hpp
    mos6502/mos6502.cpp
)

target_include_directories(assembler_utils PUBLIC
	..)

target_include_directories(mos6502 PUBLIC
	..)

target_link_libraries(mos6502 PUBLIC
	assembler_utils)
