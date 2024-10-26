#include <assembler/mos6502/mos6502.hpp>
#include <stdexcept>

namespace assembler {
namespace mos6502 {

auto mos6502Assembler::assemble(std::vector<std::string> &source)
    -> std::vector<byte_type> {
  std::vector<byte_type> result;
  for (auto &inst : source) {
    assemble(inst, result);
  }
  return result;
}

auto mos6502Assembler::assemble(std::string &inst,
                                std::vector<byte_type> &result) -> void {
  auto splitInstruction = utils::strSplit(inst);
  std::string instName{splitInstruction[0]};
  std::string operand{splitInstruction[1]};
  if (insts->contains(instName)) {
    insts->at(instName).match(operand, result);
  }
}

auto Inst::match(std::string &operand, std::vector<byte_type>& result) -> void {
  std::string_view opView{operand};
  // A trie could be optimal in the general case but given that at most there's 6 options or so per instruction it's overkill
  for (auto &possibleTypes : types_) {
    if (possibleTypes.match(opView)) {
      possibleTypes.parse(opView, result);
      return;
    }
  }
  throw std::runtime_error("Made up instruction");
}

auto mos6502Disassembler::disassemble(std::vector<byte_type>& code, std::vector<std::string>& result) -> void {
  size_t index = 0;
  const auto totalSize = code.size(); 
  while(index < code.size()) {
    index += disassemble(code, result, index);
  }
}

auto mos6502Disassembler::disassemble(std::vector<byte_type>& code, std::vector<std::string>& result, size_t index) -> size_t {
  auto opcode = code[index];
  if(insts->contains(opcode)) {
    return insts->at(opcode).disassm(code, result, index+1);
  }
  throw std::runtime_error("Made up instruction");
}

}; // namespace mos6502
}; // namespace assembler
