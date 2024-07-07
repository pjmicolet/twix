#pragma once
#include <string>
#include <stdexcept>

class TestException : public std::runtime_error {
    std::string exception_;
public:
    TestException(std::string reason) : std::runtime_error(reason) {};
};

namespace Test {
namespace Exceptions {
    inline auto require_true(const std::string statement, 
                             const int line, const bool result) -> void {
        if(!result) {
            std::string issue = ": Failed assertion \"" + statement + "\" at line " + std::to_string(line);
            throw TestException(issue);
        }
    };
};
};

#define REQUIRE_TRUE(EXPR) Test::Exceptions::require_true(#EXPR, __LINE__, (EXPR) == true);
