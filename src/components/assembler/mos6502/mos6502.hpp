#pragma once
#include <assembler/utils/asm_utils.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace assembler {
namespace mos6502 {
using byte_type = unsigned char;
using ParseFunc =
    std::function<void(std::string_view op, std::vector<byte_type> &)>;

struct OpType {
  constexpr OpType(
      const uint8_t opType, const uint8_t opCode,
      const std::string_view &opPattern,
      const ParseFunc f = [](std::string_view, std::vector<byte_type> &) {})
      : type_(opType), code_(opCode), opPattern_(opPattern), parseFunc_{f} {}

  auto getType() -> char { return code_; }

  auto match(std::string_view &v) -> bool {
    return utils::match(opPattern_, v);
  }

  auto parse(std::string_view &op, std::vector<byte_type> &data) -> void {
    data.push_back(getType());
    parseFunc_(op, data);
  }

private:
  uint8_t type_;
  uint8_t code_;
  const std::string_view &opPattern_;
  ParseFunc parseFunc_;
};

struct OpTypeEncoding {
  static inline constexpr std::string_view implicit = "";
  static inline constexpr std::string_view accumulator = "A";
  static inline constexpr std::string_view immediate = "#@byte";
  static inline constexpr std::string_view zeroPage = "$@byte";
  static inline constexpr std::string_view zeroPageX = "$@byte,X";
  static inline constexpr std::string_view zeroPageY = "$@byte,Y";
  static inline constexpr std::string_view relative = "@byte";
  static inline constexpr std::string_view absolute = "$@byte@byte";
  static inline constexpr std::string_view absoluteX = "$@byte@byte,X";
  static inline constexpr std::string_view absoluteY = "$@byte@byte,Y";
  static inline constexpr std::string_view indirect = "($@byte@byte)";
  static inline constexpr std::string_view indirectX = "($@byte,X)";
  static inline constexpr std::string_view indirectY = "($@byte),Y";
};

#define IMPL(opCode)                                                           \
  OpType { 0, opCode, OpTypeEncoding::implicit, }
#define ACC(opCode)                                                            \
  OpType { 1, opCode, OpTypeEncoding::accumulator, }
#define IMM(opCode)                                                            \
  OpType {                                                                     \
    2, opCode, OpTypeEncoding::immediate,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define ZP(opCode)                                                             \
  OpType {                                                                     \
    3, opCode, OpTypeEncoding::zeroPage,                                       \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define ZPX(opCode)                                                            \
  OpType {                                                                     \
    4, opCode, OpTypeEncoding::zeroPageX,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define ZPY(opCode)                                                            \
  OpType {                                                                     \
    5, opCode, OpTypeEncoding::zeroPageY,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define IZX(opCode)                                                            \
  OpType {                                                                     \
    6, opCode, OpTypeEncoding::indirectX,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define IZY(opCode)                                                            \
  OpType {                                                                     \
    7, opCode, OpTypeEncoding::indirectY,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = static_cast<byte_type>(utils::stringToInt(v));            \
          data.push_back(val);                                                 \
        }                                                                      \
  }
#define ABS(opCode)                                                            \
  OpType {                                                                     \
    8, opCode, OpTypeEncoding::absolute,                                       \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = utils::stringToInt(v);                                    \
          data.push_back(static_cast<byte_type>(val & 0xFF));                  \
          data.push_back(static_cast<byte_type>((val & 0xFF00) >> 8));         \
        }                                                                      \
  }
#define ABX(opCode)                                                            \
  OpType {                                                                     \
    9, opCode, OpTypeEncoding::absoluteX,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = utils::stringToInt(v);                                    \
          data.push_back(static_cast<byte_type>(val & 0xFF));                  \
          data.push_back(static_cast<byte_type>((val & 0xFF00) >> 8));         \
        }                                                                      \
  }
#define ABY(opCode)                                                            \
  OpType {                                                                     \
    10, opCode, OpTypeEncoding::absoluteY,                                     \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = utils::stringToInt(v);                                    \
          data.push_back(static_cast<byte_type>(val & 0xFF));                  \
          data.push_back(static_cast<byte_type>((val & 0xFF00) >> 8));         \
        }                                                                      \
  }
#define IND(opCode)                                                            \
  OpType {                                                                     \
    11, opCode, OpTypeEncoding::indirect,                                      \
        [](std::string_view v, std::vector<byte_type> &data) {                 \
          auto val = utils::stringToInt(v);                                    \
          data.push_back(static_cast<byte_type>(val & 0xFF));                  \
          data.push_back(static_cast<byte_type>((val & 0xFF00) >> 8));         \
        }                                                                      \
  }
#define REL(opCode)                                                            \
  OpType {                                                                     \
    12, opCode, OpTypeEncoding::relative,                                      \
        [](std::string_view, std::vector<byte_type> &) {}                      \
  }

struct Inst {
  Inst(std::string name, std::vector<OpType> types)
      : name_{name}, types{types} {}
  auto match(std::string &operand, std::vector<byte_type> &result) -> void;

private:
  std::string name_;
  std::vector<OpType> types;
};

#define INSTRUCTION(name, ...)                                                 \
  {                                                                            \
    name, Inst {                                                               \
      name, { __VA_ARGS__ }                                                    \
    }                                                                          \
  }

struct mos6502Assembler {
  mos6502Assembler() = default;
  auto assemble(std::vector<std::string> &source) -> std::vector<byte_type>;

private:
  auto assemble(std::string &inst, std::vector<byte_type> &result) -> void;
  std::unordered_map<std::string, Inst> insts = {
      INSTRUCTION("ADC", IMM(0x69), ZP(0x65), ZPX(0x75), ABS(0x6D), ABX(0x7D),
                  ABY(0x79), IZX(0x61), IZY(0x71)),
      INSTRUCTION("AND", IMM(0x29), ZP(0x25), ZPX(0x35), ABS(0x2D), ABX(0x3D),
                  ABY(0x39), IZX(0x21), IZY(0x21)),
      INSTRUCTION("ASL", ACC(0x0A), ZP(0x06), ZPX(0x16), ABS(0x0E), ABX(0x1E)),
  };
};

struct mos6502Disassembler {};

}; // namespace mos6502
}; // namespace assembler
