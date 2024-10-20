#pragma once
#include <stdexcept>
#include <string>

class TestException : public std::runtime_error {
  std::string exception_;

public:
  TestException(std::string reason) : std::runtime_error(reason) {};
};

namespace Test {
namespace Exceptions {
inline auto require_true(const std::string statement, const int line,
                         const bool result) -> void {
  if (!result) {
    std::string issue = ": Failed assertion \"" + statement + "\" at line " +
                        std::to_string(line);
    throw TestException(issue);
  }
};

template <typename T, typename R>
inline auto require_same(const int line, const T &expected, const R &got)
    -> void {
  if (expected != got) {
    std::string issue = ": Items do not match, expected=  \"" +
                        std::to_string(expected) + "\" got \"" +
                        std::to_string(got) +"\"";
    throw TestException(issue);
  }
};
}; // namespace Exceptions
}; // namespace Test

#define REQUIRE_TRUE(EXPR)                                                     \
  Test::Exceptions::require_true(#EXPR, __LINE__, (EXPR) == true);

#define REQUIRE_SAME(EXPECTED, GOT)                                            \
  Test::Exceptions::require_same(__LINE__, EXPECTED, GOT);
