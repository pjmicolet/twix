add_executable(Test asm_utils_test.cpp)
target_link_libraries(Test PUBLIC
assembler_utils test_framework)
