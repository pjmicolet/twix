#include "../../../../components/assembler/utils/asm_utils.hpp"
#include <string>
#include <iostream>

auto match_test() -> bool {
    std::string expression = "@byte@byte";
    auto match = assembler::utils::match(expression, "1234");
    return match == true;
}

int main() {
    if(match_test()) {
        std::cout << "Passed\n";
    }
    return 0;
}
