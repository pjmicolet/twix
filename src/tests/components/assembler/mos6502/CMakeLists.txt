add_executable(mos6502_test_suite test_main.cpp
  assembler.hpp
  disassembler.hpp
)

target_link_libraries(mos6502_test_suite PUBLIC
test_framework
mos6502)
