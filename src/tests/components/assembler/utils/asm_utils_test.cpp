#include "../../../../components/assembler/utils/asm_utils.hpp"
#include "../../../framework/testing.hpp"
#include <string>
#include <iostream>

TEST_CASE("match_test") {
    std::string expression = "@byte@byte";
    auto match = assembler::utils::match(expression, "1234");
}
