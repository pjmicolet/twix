#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <format>
#if __cpp_lib_print >= 202207L
#include <print>
#else
#include <iostream>
#endif
#include "registers.hpp"
#include "cycle_table.hpp"
#include <assembler/mos6502/mos6502.hpp>
#include "instructions.hpp"

namespace cores {
namespace mos6502 {

template<typename Memory>
auto printInstruction(const mos6502<Memory>& cpu) -> void {
  auto insts = assembler::mos6502::InstructionSet::getInstructionSet();
  
  uint16_t pc = cpu.R.PC;
  uint8_t opcode = cpu.mem_component.load(pc);
  
  std::string instruction_str = "???";
  
  if (insts->contains(opcode)) {
    std::vector<assembler::mos6502::byte_type> bytes;
    bytes.push_back(opcode);
    
    uint16_t temp_pc = pc + 1;
    
    if (temp_pc < 0x10000) {
      uint8_t byte1 = cpu.mem_component.load(temp_pc);
      bytes.push_back(byte1);
      
      if (temp_pc + 1 < 0x10000) {
        uint8_t byte2 = cpu.mem_component.load(temp_pc + 1);
        bytes.push_back(byte2);
      }
    }
    
    std::vector<std::string> result;
    assembler::mos6502::mos6502Disassembler disasm;
    disasm.disassemble(bytes, result);
    
    if (!result.empty()) {
      instruction_str = result[0];
    }
  }
  
  const auto& cycle_info = CYCLE_TABLE[opcode];
  std::string cycle_str;
  if (cycle_info.min == cycle_info.max) {
    cycle_str = std::format("cycles:{}", cycle_info.min);
  } else {
    cycle_str = std::format("cycles:{}-{}", cycle_info.min, cycle_info.max);
  }
  
#if __cpp_lib_print >= 202207L
  std::print("[{:04X}] {} {} {}\n", pc, instruction_str, cycle_str, cpu.R);
#else
  std::cout << std::format("[{:04X}] {} {} {}\n", pc, instruction_str, cycle_str, cpu.R);
#endif
}

}
}
