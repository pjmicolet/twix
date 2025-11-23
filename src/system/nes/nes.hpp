#pragma once
#include <cores/mos6502/cpu.hpp>

template<typename Mapper>
struct NesRAM {
  NesRAM(cores::mos6502::NesRom& rom) : rom_(rom) {
    memory_.resize(0x7FF);
  }

  auto load(uint16_t address) -> uint8_t {
    if(address <= 0x1FFF) {
      auto relativeAddress = address & 0x7FF;
      return memory_[relativeAddress];
    }
    if(address <= 0x3FFF) {
      auto relativeAddress = address & 0x7FF;
      return memory_[relativeAddress];
    }
    else {
      rom_.loadFromPrg(Mapper::compute(address));
    }
    return 0;
  }

  auto store(uint16_t address, uint8_t data) -> void {
    if(address <= 0x1FFF) {
      auto relativeAddress = address & 0x7FF;
      memory_[relativeAddress] = data;
    }
  }
  std::vector<uint8_t> memory_;
  cores::mos6502::NesRom& rom_;
};

struct Mapper0 {
  static auto compute(uint16_t addr) -> uint16_t {
    return addr & 0xBFFF;
  }
};
