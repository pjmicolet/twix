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

  auto dump() -> std::string {
    std::string mem = "memory: [";
    bool isFirst = true;
    for(auto& bin : localMem_) {
      mem += isFirst ? "" : ", ";
      mem += std::to_string(bin);
      isFirst = false;
    }
    mem += "]";
    return mem;
  }

  std::vector<uint8_t> localMem_;
};

TEST_CASE("Test LDX") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDX #10");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x10, core.getX());

  std::vector<uint8_t> bank{0x15, 0x11, 0x12, 0x13};
  core.clearState();
  listing.clear();
  listing.emplace_back("LDX $00");
  listing.emplace_back("LDX $02");
  listing.emplace_back("LDX $00,Y");
  listing.emplace_back("LDX $0003");
  listing.emplace_back("LDX $0001,Y");
  auto code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  mem.set(bank);
  core.setPC(0x4);
  core.runCycle();
  REQUIRE_SAME(0x15, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x12, core.getX());
  core.setY(1);
  core.runCycle();
  REQUIRE_SAME(0x11, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x13, core.getX());
  core.setY(2);
  core.runCycle();
  REQUIRE_SAME(0x13, core.getX());
}

TEST_CASE("Test SDY") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  cores::mos6502::mos6502 core{mem}; 

  std::vector<uint8_t> bank{0,0,0,0,0,0,0,0,0,0}; // 10 slots
  listing.emplace_back("STY $00");
  auto code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  mem.set(bank);
  core.setPC(0xA);
  core.setY(0x5);
  core.runCycle();
  REQUIRE_SAME(0x5, mem.load(0x0));
}

TEST_CASE("Test SDX") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  cores::mos6502::mos6502 core{mem}; 

  std::vector<uint8_t> bank{0,0,0,0,0,0,0,0,0,0}; // 10 slots
  listing.emplace_back("STX $00");
  listing.emplace_back("STX $01,Y");
  auto code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  mem.set(bank);
  core.setPC(0xA);
  core.setX(0x5);
  core.runCycle();
  REQUIRE_SAME(0x5, mem.load(0x0));
  core.setX(0x12);
  core.setY(0x1);
  core.runCycle();
  auto dumpInfo = [=]() mutable { return mem.dump(); };
  REQUIRE_SAME_VERBOSE(0x12, mem.load(0x2), dumpInfo);
}

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
  mem.set(bank);
  core.setPC(0x4);
  core.runCycle();
  REQUIRE_SAME(0x10, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x21, core.getAcc());
}
