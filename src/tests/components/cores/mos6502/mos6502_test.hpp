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
  listing.emplace_back("STX $0003");
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
  REQUIRE_SAME(0x12, mem.load(0x2));
  core.setX(0x22);
  core.runCycle();
  REQUIRE_SAME(0x22, mem.load(0x3));
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

TEST_CASE("Test SDC") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("ADC #10");
  listing.emplace_back("SBC #01");
  listing.emplace_back("SBC #0E"); // That will do 0xE - 0xE - (1 - 0) so it should wrap around to 0xFF with Carry set to 1
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0xE, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0xFF, core.getAcc());
  REQUIRE_SAME(0x1, core.getCarry());
}

TEST_CASE("SET AND CLEAR INSTRUCTIONS") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("SEC");
  listing.emplace_back("SED");
  listing.emplace_back("SEI");
  listing.emplace_back("CLC");
  listing.emplace_back("CLI");
  listing.emplace_back("CLD");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(1, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(1, core.getDig());
  core.runCycle();
  REQUIRE_SAME(1, core.getInt());
  core.runCycle();
  REQUIRE_SAME(0, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(0, core.getInt());
  core.runCycle();
  REQUIRE_SAME(0, core.getDig());
}

TEST_CASE("AND") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #10");
  listing.emplace_back("AND #10");
  listing.emplace_back("AND #00");
  listing.emplace_back("LDA #10");
  listing.emplace_back("BIT $00"); //Doesn't really make sense but #10 isn't going to be LDA's hex code
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x10, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  REQUIRE_SAME(0x1, core.getZero());
  core.runCycle();
  REQUIRE_SAME(0x0, core.getZero());
  core.runCycle();
  REQUIRE_SAME(0x1, core.getZero());
}

TEST_CASE("EOR and ORA") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #0F");
  listing.emplace_back("EOR #FF");
  listing.emplace_back("LDA #0F");
  listing.emplace_back("ORA #F0");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x0F, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0xF0, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x0F, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0xFF, core.getAcc());
}

TEST_CASE("CMP, CPX, CPY") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #10");
  listing.emplace_back("CMP #10");
  listing.emplace_back("CMP #05");
  listing.emplace_back("CMP #20");
  listing.emplace_back("LDX #10");
  listing.emplace_back("CPX #10");
  listing.emplace_back("CPX #05");
  listing.emplace_back("LDY #10");
  listing.emplace_back("CPY #10");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x1, core.getZero());
  REQUIRE_SAME(0x1, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(0x0, core.getZero());
  REQUIRE_SAME(0x1, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(0x0, core.getZero());
  REQUIRE_SAME(0x0, core.getCarry());
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x1, core.getZero());
  REQUIRE_SAME(0x1, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(0x1, core.getCarry());
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x1, core.getZero());
}

TEST_CASE("INC and DEC") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  std::vector<uint8_t> bank{0x10, 0xFF, 0x00, 0x01};
  listing.emplace_back("INC $00");
  listing.emplace_back("INC $01");
  listing.emplace_back("DEC $02");
  listing.emplace_back("DEC $03");
  auto code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  mem.set(bank);
  cores::mos6502::mos6502 core{mem}; 
  core.setPC(0x4);
  core.runCycle();
  REQUIRE_SAME(0x11, mem.load(0x00));
  core.runCycle();
  REQUIRE_SAME(0x00, mem.load(0x01));
  REQUIRE_SAME(0x1, core.getZero());
  core.runCycle();
  REQUIRE_SAME(0xFF, mem.load(0x02));
  core.runCycle();
  REQUIRE_SAME(0x00, mem.load(0x03));
  REQUIRE_SAME(0x1, core.getZero());
}

TEST_CASE("INX, INY, DEX, DEY") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDX #10");
  listing.emplace_back("INX");
  listing.emplace_back("LDY #20");
  listing.emplace_back("INY");
  listing.emplace_back("DEX");
  listing.emplace_back("DEY");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x10, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x11, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x20, core.getY());
  core.runCycle();
  REQUIRE_SAME(0x21, core.getY());
  core.runCycle();
  REQUIRE_SAME(0x10, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x20, core.getY());
}

TEST_CASE("ASL and LSR") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #02");
  listing.emplace_back("ASL A");
  listing.emplace_back("ASL A");
  listing.emplace_back("LSR A");
  listing.emplace_back("LSR A");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x02, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x04, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x08, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x04, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x02, core.getAcc());
}

TEST_CASE("ROL and ROR") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #81");
  listing.emplace_back("ROL A");
  listing.emplace_back("ROL A");
  listing.emplace_back("ROR A");
  listing.emplace_back("ROR A");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x81, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x02, core.getAcc());
  REQUIRE_SAME(0x1, core.getCarry());
  core.runCycle();
  REQUIRE_SAME(0x05, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x02, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x81, core.getAcc());
}

TEST_CASE("Branches") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #00");
  auto code = assembler.assemble(listing);
  std::vector<uint8_t> bank;
  bank.insert(bank.end(), code.begin(), code.end());
  bank.push_back(0xF0);
  bank.push_back(0x02);
  listing.clear();
  listing.emplace_back("LDA #FF");
  code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  listing.clear();
  listing.emplace_back("LDA #10");
  code = assembler.assemble(listing);
  bank.insert(bank.end(), code.begin(), code.end());
  mem.set(bank);
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  REQUIRE_SAME(0x1, core.getZero());
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x10, core.getAcc());
}

TEST_CASE("Transfer Instructions") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #42");
  listing.emplace_back("TAX");
  listing.emplace_back("TAY");
  listing.emplace_back("LDA #00");
  listing.emplace_back("TXA");
  listing.emplace_back("LDA #00");
  listing.emplace_back("TYA");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getX());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getY());
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
}

TEST_CASE("Stack Instructions") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("LDA #42");
  listing.emplace_back("PHA");
  listing.emplace_back("LDA #00");
  listing.emplace_back("PLA");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
}

TEST_CASE("JMP and JSR/RTS") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  std::vector<uint8_t> bank(256, 0xEA);
  bank[0] = 0x4C;
  bank[1] = 0x10;
  bank[2] = 0x00;
  listing.emplace_back("LDA #42");
  auto code = assembler.assemble(listing);
  for(size_t i = 0; i < code.size(); i++) {
    bank[0x10 + i] = code[i];
  }
  mem.set(bank);
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
}

TEST_CASE("NOP") {
  LocalMem mem{};
  std::vector<std::string> listing{};
  assembler::mos6502::mos6502Assembler assembler{};
  listing.emplace_back("NOP");
  listing.emplace_back("LDA #42");
  mem.set(assembler.assemble(listing));
  cores::mos6502::mos6502 core{mem}; 
  core.runCycle();
  REQUIRE_SAME(0x00, core.getAcc());
  core.runCycle();
  REQUIRE_SAME(0x42, core.getAcc());
}
