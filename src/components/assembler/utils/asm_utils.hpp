#pragma once
#include <vector>
#include <string_view>

namespace assembler { namespace utils {
    //Non constexpr form :)
    auto strSplit(const std::string_view strv, std::string_view delim = " ") -> std::vector<std::string_view>;

    auto isHex(const char& c) -> bool;

    auto match(const std::string_view pattern, const std::string_view testString) -> bool;

    //This is dirty but it will clear up the string and then create the int
    auto stringToInt(const std::string_view pattern) -> int;
}; };
