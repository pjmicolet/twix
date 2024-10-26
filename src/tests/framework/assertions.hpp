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
    std::string expectedS;
    std::string gotS;

    if constexpr(std::is_integral<T>::value) {
      expectedS = std::to_string(expected);
    } else {
      expectedS = std::string(expected);
    }

    if constexpr(std::is_integral<R>::value) {
      gotS = std::to_string(got);
    } else {
      gotS = std::string(got);
    }

    std::string issue = ": Items do not match, expected=  \"" +
                        expectedS + "\" got \"" +
                        gotS +"\"";
    throw TestException(issue);
  }
};
}; // namespace Exceptions
}; // namespace Test

#define REQUIRE_TRUE(EXPR)                                                     \
  Test::Exceptions::require_true(#EXPR, __LINE__, (EXPR) == true);

#define REQUIRE_SAME(EXPECTED, GOT)                                            \
  Test::Exceptions::require_same(__LINE__, EXPECTED, GOT);
