#include <mos6502/instructions.hpp>
#include <assembler/mos6502/mos6502.hpp>
#include <framework/testing.hpp>
#include <vector>
#include <cstdint>
#include <string>


struct LocalMem {
  auto load(uint16_t addr) -> uint8_t {
    return localMem_[addr];
  }

  auto store(uint16_t addr, uint8_t contents) -> void {
    localMem_[addr] = contents;
  }

  auto set(std::vector<uint8_t>&& data) {
    localMem_ = data;
  }
  
  auto set(std::vector<uint8_t>& data) {
    localMem_ = data;
  }

  std::vector<uint8_t> localMem_;
};

TEST_CASE("Test ADC") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("ADC #10");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x10, core.getAcc());

  std::vector<uint8_t> bank{0x10, 0x11, 0x12, 0x13};
  core.clearState();
  listing.clear();
  listing.emplace_back("ADC $00");
  listing.emplace_back("ADC $01");
  listing.emplace_back("ADC $02");
  listing.emplace_back("ADC $03");
  auto code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  for(auto& c : bank) {
    std::cout << std::hex << static_cast<uint16_t>(c) << " ";
  }
  std::cout << "\n";
  mem.set(bank);
  core.setPC(0x4);
  core.runCycle();
  REQUIRE_SAME(0x10, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x21, core.getAcc());
}
