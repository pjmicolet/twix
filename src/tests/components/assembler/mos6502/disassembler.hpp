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
}
