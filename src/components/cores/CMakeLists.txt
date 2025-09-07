add_library(mos6502core
	mos6502/instructions.hpp
	mos6502/instructions.cpp
)

target_include_directories(mos6502core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(mos6502core INTERFACE SYSTEM
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
