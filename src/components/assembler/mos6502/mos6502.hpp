#pragma once
#include <assembler/utils/asm_utils.hpp>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace assembler {
namespace mos6502 {
using byte_type = unsigned char;

// How the rest of the data should be parsed for the instruction (the bit that's
// not a name)
enum ParseType { NONE, SINGLE, DOUBLE };

enum Ops { IMPL, ACC, IMM, ZP, ZPX, ZPY, IZX, IZY, ABS, ABX, ABY, IND, REL };

// Represents all the different types instructions can be (immediate, Absolute
// X, Absolute Y) Instructions are basically just the opcode + this optype
struct OpType {
  constexpr OpType(const Ops opType, const uint8_t opCode,
                   const std::string_view &opPattern, const ParseType f = NONE)
      : type_(opType), code_(opCode), opPattern_(opPattern), parseType_{f} {}

  auto getType() -> char { return code_; }

  auto match(std::string_view &v) -> bool {
    return utils::match(opPattern_, v);
  }

  auto parse(std::string_view &op, std::vector<byte_type> &data) -> void {
    data.push_back(getType());
    switch (parseType_) {
    case ParseType::SINGLE: {
      parseOne(op, data);
      break;
    }
    case ParseType::DOUBLE: {
      parseTwo(op, data);
      break;
    }
    case ParseType::NONE: {
      break;
    }
    }
  }

  auto disassm(std::vector<byte_type> &bytes, std::vector<std::string> &result,
               size_t index) -> int {
    switch (parseType_) {
    case ParseType::SINGLE: {
      auto strData = std::format("{:02X}", bytes[index]);
      disasmAddressing(strData, result);
      return index + 1;
    }
    case ParseType::DOUBLE: {
      auto strData = std::format("{:02X}", bytes[index+1]);
      auto strData2 = std::format("{:02X}", bytes[index]);
      auto res = strData + strData2;
      disasmAddressing(res, result);
      return index + 2;
    }
    case ParseType::NONE: {
      result.push_back(name_);
      return index;
      }
    }
    return 0;
  }

  auto setName(std::string name) { name_ = name; }

  auto getNumOfBytes() -> int { 
    if(parseType_ == ParseType::SINGLE) return 1;
    if(parseType_ == ParseType::DOUBLE) return 2; 
    return 0;
  }

private:
  Ops type_;
  uint8_t code_;
  std::string_view opPattern_; // used to be const but makes a mess
  ParseType parseType_;
  std::string name_;

  auto parseOne(std::string_view &op, std::vector<byte_type> &data) -> void {
    auto val = static_cast<byte_type>(utils::stringToInt(op));
    data.push_back(val);
  }

  auto parseTwo(std::string_view &op, std::vector<byte_type> &data) -> void {
    auto val = utils::stringToInt(op);
    data.push_back(static_cast<byte_type>(val & 0xFF));
    data.push_back(static_cast<byte_type>((val & 0xFF00) >> 8));
  }

  auto disasmAddressing(std::string &strData, std::vector<std::string> &result)
      -> void {
    switch (type_) {
    case Ops::IMM: {
      result.emplace_back(name_ + " #" + strData);
      break;
    }
    case Ops::ZP: {
      result.emplace_back(name_ + " $" + strData);
      break;
    }
    case Ops::ZPX: {
      result.emplace_back(name_ + " $" + strData + ",X");
      break;
    }
    case Ops::ZPY: {
      result.emplace_back(name_ + " $" + strData + ",Y");
      break;
    }
    case Ops::IZX: {
      result.emplace_back(name_ + " $(" + strData + ",X)");
      break;
    }
    case Ops::IZY: {
      result.emplace_back(name_ + " $(" + strData + "),Y");
      break;
    }
    case Ops::ABS: {
      result.emplace_back(name_ + " $" + strData);
      break;
    }
    case Ops::ABX: {
      result.emplace_back(name_ + " $" + strData + ",X");
      break;
    }
    case Ops::ABY: {
      result.emplace_back(name_ + " $" + strData + ",Y");
      break;
    }
    case Ops::IND: {
      result.emplace_back(name_ + " ($" + strData + ")");
      break;
    }
    default:
      break;
    }
  }
};

// Used for pattern matching
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

#define SINGLE_BYTE_PARSE ParseType::SINGLE

#define DOUBLE_BYTE_PARSE ParseType::DOUBLE

#define IMPL(opCode)                                                           \
  OpType { Ops::IMPL, opCode, OpTypeEncoding::implicit, }
#define ACC(opCode)                                                            \
  OpType { Ops::ACC, opCode, OpTypeEncoding::accumulator, }
#define IMM(opCode)                                                            \
  OpType { Ops::IMM, opCode, OpTypeEncoding::immediate, SINGLE_BYTE_PARSE }
#define ZP(opCode)                                                             \
  OpType { Ops::ZP, opCode, OpTypeEncoding::zeroPage, SINGLE_BYTE_PARSE }
#define ZPX(opCode)                                                            \
  OpType { Ops::ZPX, opCode, OpTypeEncoding::zeroPageX, SINGLE_BYTE_PARSE }
#define ZPY(opCode)                                                            \
  OpType { Ops::ZPY, opCode, OpTypeEncoding::zeroPageY, SINGLE_BYTE_PARSE }
#define IZX(opCode)                                                            \
  OpType { Ops::IZX, opCode, OpTypeEncoding::indirectX, SINGLE_BYTE_PARSE }
#define IZY(opCode)                                                            \
  OpType { Ops::IZY, opCode, OpTypeEncoding::indirectY, SINGLE_BYTE_PARSE }
#define ABS(opCode)                                                            \
  OpType { Ops::ABS, opCode, OpTypeEncoding::absolute, DOUBLE_BYTE_PARSE }
#define ABX(opCode)                                                            \
  OpType { Ops::ABX, opCode, OpTypeEncoding::absoluteX, DOUBLE_BYTE_PARSE }
#define ABY(opCode)                                                            \
  OpType { Ops::ABY, opCode, OpTypeEncoding::absoluteY, DOUBLE_BYTE_PARSE }
#define IND(opCode)                                                            \
  OpType { Ops::IND, opCode, OpTypeEncoding::indirect, DOUBLE_BYTE_PARSE }
#define REL(opCode)                                                            \
  OpType { Ops::REL, opCode, OpTypeEncoding::relative }

struct InstructionSet;

struct Inst {
  Inst(std::string name, std::vector<OpType> types)
      : name_{name}, types_{types} {
    for (auto &t : types_) {
      t.setName(name_);
    }
  }
  auto match(std::string &operand, std::vector<byte_type> &result) -> void;
  friend InstructionSet;

protected:
  std::string name_;
  std::vector<OpType> types_;
};

#define INSTRUCTION(name, ...)                                                 \
  {                                                                            \
    name, Inst {                                                               \
      name, { __VA_ARGS__ }                                                    \
    }                                                                          \
  }

/**
 * MultiKey structure, you can either access it by a byte which is a specific
 * opcode or by an instruction name
 *
 */
struct InstructionSet {
  static auto getInstructionSet() -> std::shared_ptr<InstructionSet> {
    static std::shared_ptr<InstructionSet> inst{new InstructionSet};
    return inst;
  }

  InstructionSet(InstructionSet const &) = delete;
  void operator=(InstructionSet const &) = delete;

  auto contains(const std::string &name) -> bool {
    return byName_.contains(name);
  }

  auto contains(byte_type op) -> bool { return byOp_.contains(op); }

  auto at(const std::string &name) -> Inst & { return byName_.at(name); }

  auto at(byte_type op) -> OpType & { return *byOp_.at(op); }

  auto getOpType(byte_type op) -> int { return byOp_.at(op)->getNumOfBytes(); }

private:
  InstructionSet() {
    byName_ = {
        INSTRUCTION("ADC", IMM(0x69), ZP(0x65), ZPX(0x75), ABS(0x6D), ABX(0x7D),
                    ABY(0x79), IZX(0x61), IZY(0x71)),
        INSTRUCTION("AND", IMM(0x29), ZP(0x25), ZPX(0x35), ABS(0x2D), ABX(0x3D),
                    ABY(0x39), IZX(0x21), IZY(0x21)),
        INSTRUCTION("ASL", ACC(0x0A), ZP(0x06), ZPX(0x16), ABS(0x0E),
                    ABX(0x1E)),
        INSTRUCTION("BCC", REL(0x90)),
        INSTRUCTION("BCS", REL(0xB0)),
        INSTRUCTION("BEQ", REL(0xF0)),
        INSTRUCTION("BIT", ZP(0x24), ABS(0x2C)),
        INSTRUCTION("BMI", REL(0x30)),
        INSTRUCTION("BNE", REL(0xD0)),
        INSTRUCTION("BPL", REL(0x10)),
        INSTRUCTION("BRK", IMPL(0x00)),
        INSTRUCTION("BVC", REL(0x50)),
        INSTRUCTION("BVS", REL(0x70)),
        INSTRUCTION("CLC", IMPL(0x18)),
        INSTRUCTION("CLD", IMPL(0xD8)),
        INSTRUCTION("CLI", IMPL(0x58)),
        INSTRUCTION("CLV", IMPL(0xB8)),
        INSTRUCTION("CMP", IMM(0xC9), ZP(0xC5), ZPX(0xD5), ABS(0xCD), ABX(0xDD),
                    ABY(0xD9), IZX(0xC1), IZY(0xD1)),
        INSTRUCTION("CPX", IMM(0xE0), ZP(0xE4), ABS(0xEC)),
        INSTRUCTION("CPY", IMM(0xC0), ZP(0xC4), ABS(0xCC)),
        INSTRUCTION("DEC", ZP(0xC6), ZPX(0xD6), ABS(0xCE), ABX(0xDE)),
        INSTRUCTION("DEX", IMPL(0xCA)),
        INSTRUCTION("DEY", IMPL(0x88)),
        INSTRUCTION("EOR", IMM(0x49), ZP(0x45), ZPX(0x55), ABS(0x4D), ABX(0x5D),
                    ABY(0x59), IZX(0x41), IZY(0x51)),
        INSTRUCTION("INC", ZP(0xE6), ZPX(0xF6), ABS(0xEE), ABX(0xFE)),
        INSTRUCTION("INX", IMPL(0xE8)),
        INSTRUCTION("INY", IMPL(0xC8)),
        INSTRUCTION("JMP", ABS(0x4C), IND(0x6C)),
        INSTRUCTION("JSR", ABS(0x20)),
        INSTRUCTION("LDA", IMM(0xA9), ZP(0xA5), ZPX(0xB5), ABS(0xAD), ABX(0xBD),
                    ABY(0xB9), IZX(0xA1), IZY(0xA1)),
        INSTRUCTION("LDX", IMM(0xA2), ZP(0xA6), ZPX(0xB6), ABS(0xAE),
                    ABY(0xBE)),
        INSTRUCTION("LDY", IMM(0xA0), ZP(0xA4), ZPX(0xB4), ABS(0xBE),
                    ABX(0xBC)),
        INSTRUCTION("LSR", ACC(0x4A), ZP(0x46), ZPX(0x56), ABS(0x4E),
                    ABX(0x5E)),
        INSTRUCTION("NOP", IMPL(0xEA)),
        INSTRUCTION("ORA", IMM(0x09), ZP(0x05), ZPX(0x15), ABS(0x0D), ABX(0x1D),
                    ABY(0x19), IZX(0x01), IZY(0x11)),
        INSTRUCTION("PHA", IMPL(0x48)),
        INSTRUCTION("PHP", IMPL(0x08)),
        INSTRUCTION("PLA", IMPL(0x68)),
        INSTRUCTION("PLP", IMPL(0x28)),
        INSTRUCTION("ROL", ACC(0x2A), ZP(0x26), ZPX(0x36), ABS(0x2E),
                    ABX(0x3E)),
        INSTRUCTION("ROR", ACC(0x6A), ZP(0x66), ZPX(0x76), ABS(0x6E),
                    ABX(0x7E)),
        INSTRUCTION("RTI", IMPL(0x40)),
        INSTRUCTION("RTS", IMPL(0x60)),
        INSTRUCTION("SBC", IMM(0xE9), ZP(0xE5), ZPX(0xF5), ABS(0xED), ABX(0xFD),
                    ABY(0xF9), IZX(0xE1), IZY(0xF1)),
        INSTRUCTION("SEC", IMPL(0x38)),
        INSTRUCTION("SED", IMPL(0xF8)),
        INSTRUCTION("SEI", IMPL(0x78)),
        INSTRUCTION("STA", ZP(0x85), ZPX(0x95), ABS(0x8D), ABX(0x9D), ABY(0x99),
                    IZX(0x81), IZY(0x91)),
        INSTRUCTION("STX", ZP(0x86), ZPY(0x96), ABS(0x8E)),
        INSTRUCTION("STY", ZP(0x84), ZPX(0x94), ABS(0x8C)),
        INSTRUCTION("TAX", IMPL(0xAA)),
        INSTRUCTION("TAY", IMPL(0xA8)),
        INSTRUCTION("TSX", IMPL(0xBA)),
        INSTRUCTION("TXA", IMPL(0x8A)),
        INSTRUCTION("TXS", IMPL(0x9A)),
        INSTRUCTION("TYA", IMPL(0x98)),
    };

    for (auto &inst : byName_) {
      for (auto &op : inst.second.types_) {
        byOp_[op.getType()] = &op;
      }
    }
  }

  std::unordered_map<std::string, Inst> byName_;
  std::unordered_map<byte_type, OpType *> byOp_;
};

struct mos6502Assembler {
  mos6502Assembler() { insts = InstructionSet::getInstructionSet(); }
  auto assemble(std::vector<std::string> &source) -> std::vector<byte_type>;

private:
  auto assemble(std::string &inst, std::vector<byte_type> &result) -> void;
  std::shared_ptr<InstructionSet> insts;
};

struct mos6502Disassembler {
  mos6502Disassembler() { insts = InstructionSet::getInstructionSet(); }
  auto disassemble(std::vector<byte_type> &code,
                   std::vector<std::string> &result) -> void;

private:
  auto disassemble(std::vector<byte_type> &code,
                   std::vector<std::string> &result, size_t index) -> size_t;
  std::shared_ptr<InstructionSet> insts{};
};

} // namespace mos6502
} // namespace assembler
