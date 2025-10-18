#include "asm_utils.hpp"
#include <cstdint>
#include <string>

namespace assembler {
namespace utils {
// Non constexpr form :)
auto strSplit(const std::string_view strv, std::string_view delim /*=""*/)
    -> std::vector<std::string_view> {
  std::vector<std::string_view> output;
  size_t first = 0;
  while (first < strv.size()) {
    const auto second = strv.find_first_of(delim, first);
    if (first == 0 && second == std::string_view::npos)
      output.push_back(strv);
    else if (first != second) {
      output.push_back(strv.substr(first, second - first));
    }
    if (second == std::string_view::npos)
      break;
    first = second + 1;
  }
  return output;
}

auto isHex(const char &c) -> bool {
  return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/**
 * Pass a match pattern and the string you want to match
 * There's some tags you can use to make it generic
 **/
auto match(const std::string_view pattern, const std::string_view testString)
    -> bool {
  // formalise some basic pattern match language, maybe something like
  // @name $(@number),Y will be true for LDA $(123),Y
  // const auto splitPattern = strSplit<size>(pattern);
  // auto splitTest = strSplit<size>(testString);
  size_t pattern_index = 0;
  size_t i = 0;

  while (pattern_index < pattern.size() || i < testString.size()) {
    if (i >= testString.size() && pattern_index < pattern.size())
      return false;
    if ((pattern_index < pattern.size() - 3) && pattern[pattern_index] == '@' &&
        pattern[pattern_index + 1] == 'n') {
      pattern_index = pattern_index + 5;
      bool tilEnd = (pattern_index >= pattern.size());

      while (
          (tilEnd && i < testString.size()) ||
          (i < testString.size() && testString[i] != pattern[pattern_index])) {
        if (!((testString[i] > 64 && testString[i] < 91) ||
              (testString[i] > 96 && testString[i] < 123)))
          return false;
        i++;
      }
      continue;
    } //@digit tag
    else if ((pattern_index < pattern.size() - 3) &&
             pattern[pattern_index] == '@' &&
             pattern[pattern_index + 1] == 'd') {
      pattern_index = pattern_index + 7;
      bool tilEnd = (pattern_index >= pattern.size());

      while (
          (tilEnd && i < testString.size()) ||
          (i < testString.size() && testString[i] != pattern[pattern_index])) {
        if (testString[i] < 48 || testString[i] > 57)
          return false;
        i++;
      }
      continue;
    }
    // This is how I can help parse between say #01 and #01234
    // You can just do
    // #@byte
    // #@byte@byte
    else if ((pattern_index < pattern.size() - 3) &&
             pattern[pattern_index] == '@' &&
             pattern[pattern_index + 1] == 'b') {
      pattern_index = pattern_index + 5;
      bool tilEnd = (pattern_index >= pattern.size());
      uint8_t bytes = 2;
      while (bytes > 0 && ((tilEnd && i < testString.size()) ||
                           (testString[i] != pattern[pattern_index]))) {
        if (!isHex(testString[i]))
          return false;
        i++;
        bytes--;
      }
      continue;
    } else if (i >= testString.size() || pattern_index >= pattern.size())
      return false;
    else if (pattern[pattern_index] != testString[i])
      return false;
    i++;
    pattern_index++;
  }
  return true;
}

// This is dirty but it will clear up the string and then create the int
auto stringToInt(const std::string_view pattern) -> int {
  int res = 0;
  bool foundYet = false;
  
  for (const auto c : pattern) {
    if (isHex(c)) {
      foundYet = true;
      res = res * 16;
      if (c >= '0' && c <= '9') {
        res += (c - '0');
      } else if (c >= 'A' && c <= 'F') {
        res += (c - 'A' + 10);
      } else if (c >= 'a' && c <= 'f') {
        res += (c - 'a' + 10);
      }
    } else if (foundYet) {
      break;
    }
  }
  
  return res;
}

}; // namespace utils
}; // namespace assembler
