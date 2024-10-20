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
  if (insts.contains(instName)) {
    insts.at(instName).match(operand, result);
  }
}

auto Inst::match(std::string &operand, std::vector<byte_type>& result) -> void {
  std::string_view opView{operand};
  for (auto &possibleTypes : types) {
    if (possibleTypes.match(opView)) {
      possibleTypes.parse(opView, result);
      return;
    }
  }
  throw std::runtime_error("Made up instruction");
}
}; // namespace mos6502
}; // namespace assembler
