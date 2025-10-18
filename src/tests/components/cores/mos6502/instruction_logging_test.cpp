#include <cores/mos6502/instructions.hpp>
#include <cores/mos6502/instructions_format.hpp>
#include <cores/mos6502/memory.hpp>
#include <iostream>

struct LocalMem {
  std::vector<uint8_t> mem;
  
  LocalMem() : mem(0x10000, 0) {}
  
  auto load(uint16_t address) const -> uint8_t {
    return mem[address];
  }
  
  auto store(uint16_t address, uint8_t value) -> void {
    mem[address] = value;
  }
  
  auto set(uint16_t address, std::vector<uint8_t> data) -> void {
    for (size_t i = 0; i < data.size(); i++) {
      mem[address + i] = data[i];
    }
  }
};

int main() {
  LocalMem mem;
  cores::mos6502::mos6502<LocalMem> cpu(mem);
  
  std::cout << "=== MOS 6502 Instruction Logging Demo ===\n\n";
  
  mem.set(0x0000, {0xA9, 0x42});
  cpu.setPC(0x0000);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - ACC should be 0x42\n\n";
  
  mem.set(0x0002, {0xA2, 0x10});
  cpu.setPC(0x0002);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - X should be 0x10\n\n";
  
  mem.set(0x0004, {0xA0, 0x20});
  cpu.setPC(0x0004);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - Y should be 0x20\n\n";
  
  mem.set(0x0006, {0x69, 0x08});
  cpu.setPC(0x0006);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - ACC should be 0x4A (0x42 + 0x08)\n\n";
  
  mem.set(0x0050, {0xFF});
  mem.set(0x0008, {0xA5, 0x50});
  cpu.setPC(0x0008);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - ACC should be 0xFF (loaded from $50)\n\n";
  
  mem.set(0x000A, {0x85, 0x60});
  cpu.setPC(0x000A);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - Memory at $60 should be 0xFF\n\n";
  
  mem.set(0x1234, {0xAA});
  mem.set(0x000C, {0xAD, 0x34, 0x12});
  cpu.setPC(0x000C);
  std::cout << "Before: ";
  cores::mos6502::printInstruction(cpu);
  cpu.runCycle();
  std::cout << "After execution - ACC should be 0xAA (loaded from $1234)\n\n";
  
  std::cout << "=== Demo Complete ===\n";
  
  return 0;
}
