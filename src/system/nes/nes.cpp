#include <cores/mos6502/cpu.hpp>
#include <print>
#include "nes.hpp"

int main(int argc, char** argv) {
  std::string rom_path = argv[1];
  std::print("Hey this is {}", rom_path);
  cores::mos6502::NesRom rom{rom_path};
  NesRAM<Mapper0> ram{rom};
  cores::mos6502::mos6502<NesRAM<Mapper0>> core{ram};
  core.setPC(0xC000);
  cores::mos6502::printInstruction(core);
  //while(true) {
  //  cores::mos6502::printInstruction(core);
  //  core.runCycle();
  //}
  return 0;
}
