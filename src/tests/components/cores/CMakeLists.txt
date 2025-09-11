add_executable(mos6502_core_test_suite mos6502/test_main.cpp
	mos6502/mos6502_test.hpp
)

target_link_libraries(mos6502_core_test_suite PUBLIC
	test_framework
	mos6502core
  mos6502
)
