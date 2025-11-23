#include <cores/mos6502/cpu.hpp>
#include "nes.hpp"

int main(int argc, char** argv) {
  std::string rom_path = argv[1];
  cores::mos6502::NesRom rom{rom_path};
  NesRAM<Mapper0> ram{rom};
  cores::mos6502::mos6502<NesRAM<Mapper0>> core{ram};
  core.setPC(0xC000);
  while(true) {
    cores::mos6502::printInstruction(core);
    core.runCycle();
  }
  return 0;
}
