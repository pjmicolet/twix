#include <framework/testing.hpp>
#include <assembler/mos6502/mos6502.hpp>
#include <vector>
#include <string>

TEST_CASE("Basic 6502 Disassembly") {
  std::vector<assembler::mos6502::byte_type> bytes{0x69,0x2};
  std::vector<std::string> results{};

  assembler::mos6502::mos6502Disassembler disassm{};

  disassm.disassemble(bytes, results);
  REQUIRE_SAME(1,results.size());
  REQUIRE_SAME("ADC #02", results[0]);

  results.clear();
  bytes.push_back(0x71);
  bytes.push_back(0xFF);
  disassm.disassemble(bytes, results);
  REQUIRE_SAME(2, results.size());
  REQUIRE_SAME("ADC $(FF),Y", results[1]);

  results.clear();
  bytes.push_back(0x6D);
  bytes.push_back(0xAB);
  bytes.push_back(0xCD);
  disassm.disassemble(bytes, results);
  REQUIRE_SAME(3, results.size());
  REQUIRE_SAME("ADC $CDAB", results[2]);

  results.clear();
  bytes.push_back(0x79);
  bytes.push_back(0x0);
  bytes.push_back(0x0);

  disassm.disassemble(bytes, results);
  REQUIRE_SAME(4, results.size());
  REQUIRE_SAME("ADC $0000,Y", results[3]);
}
