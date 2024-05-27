#include "../../../../components/assembler/utils/asm_utils.hpp"
#include <string>
#include <vector>

int main() {
    constexpr std::string example = std::string("a,b,c,d,e");
    constexpr std::vector<std::string_view> delim = strSplit(example, ",");
    return delim.size() > 1;
}
