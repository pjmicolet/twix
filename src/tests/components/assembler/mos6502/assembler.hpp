#include <framework/testing.hpp>
#include <assembler/mos6502/mos6502.hpp>
#include <vector>
#include <string>

#define RESET(lst, str) lst.clear();\
  lst.emplace_back(str);\

TEST_CASE("Basic 6502 Assembly") {
  std::vector<std::string> listing{};
  listing.emplace_back("ADC #12");

  assembler::mos6502::mos6502Assembler assembler{};
  auto data = assembler.assemble(listing);

  REQUIRE_TRUE(data.size() == 2);
  REQUIRE_TRUE(data[0] == 0x69);
  REQUIRE_TRUE(data[1] == 0x12);

  RESET(listing,"ADC $1234");

  data = assembler.assemble(listing);

  REQUIRE_TRUE(data.size() == 3);
  REQUIRE_TRUE(data[0] == 0x6D);
  REQUIRE_TRUE(data[1] == 0x34);
  REQUIRE_TRUE(data[2] == 0x12);

  RESET(listing, "ADC $5678,X");

  data = assembler.assemble(listing);

  REQUIRE_TRUE(data.size() == 3);
  REQUIRE_TRUE(data[0] == 0x7D);
  REQUIRE_TRUE(data[1] == 0x78);
  REQUIRE_TRUE(data[2] == 0x56);

  RESET(listing, "ADC $9988,Y");

  data = assembler.assemble(listing);

  REQUIRE_SAME(3, data.size());
  REQUIRE_SAME(0x79, data[0]);
  REQUIRE_SAME(0x88, data[1]);
  REQUIRE_SAME(0x99, data[2]);

  RESET(listing, "ADC ($FA,X)")
  data = assembler.assemble(listing);

  REQUIRE_SAME(2, data.size());
  REQUIRE_SAME(0x61, data[0]);
  REQUIRE_SAME(0xFA, data[1]);

  RESET(listing, "ADC ($32),Y")
  data = assembler.assemble(listing);

  REQUIRE_SAME(2, data.size());
  REQUIRE_SAME(0x71, data[0]);
  REQUIRE_SAME(0x32, data[1]);

  RESET(listing, "SEC");
  data = assembler.assemble(listing);
  REQUIRE_SAME(1, data.size());
  REQUIRE_SAME(0x38, data[0]);
}
