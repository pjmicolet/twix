#pragma once

enum MemoryAction {
  IsLoad,
  IsStore
};

//The template signature has MemoryAction because all instructions just take a uint16_t as an input
//and for most instructions you just want to get whatever you're acting on in the input
//so say you have ADC $0000 you don't want to have to first take $0000 then call load then do your add
//instead you get whatever is in $0000 and do your business.
//This leads to an awkward situation where STA $0000 you want to *actually* take $0000, so the MemoryAction
//lets us pass addresses for store-based instructions and actual data for all other instructions.

struct Implied {
    template <MemoryAction m, typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      return 2 + (cpu.*instruction)(0);
    }
};

struct ImmediateMode {
    template <MemoryAction m, typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      uint8_t mem = cpu.load(cpu.nextByte());
      return 2 + (cpu.*instruction)(mem);
    }
};

struct ZPX {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      uint16_t mem = static_cast<uint16_t>(cpu.getX() + cpu.load(cpu.nextByte()))&0xFF;
      if constexpr(m == IsLoad) {
        uint8_t data = cpu.load(mem);
        return 4 + (cpu.*instruction)(data);
      } else {
        return 4 + (cpu.*instruction)(mem);
      }
    }
};

struct ZPY {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      uint16_t mem = static_cast<uint16_t>(cpu.getY() + cpu.load(cpu.nextByte()))&0xFF;
      if constexpr(m == IsLoad) {
        uint8_t data = cpu.load(mem);
        return 4 + (cpu.*instruction)(data);
      } else {
        return 4 + (cpu.*instruction)(mem);
      }
    }
};


struct ZP {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto page = cpu.load(cpu.nextByte());
      if constexpr(m == IsLoad) {
        return 3 + (cpu.*instruction)(cpu.load(page));
      } else {
        return 3 + (cpu.*instruction)(page);
      }
    }
};

struct AbsAddress {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      if constexpr(m == IsLoad) {
        auto data = cpu.load((highByte << 8) | lowByte);
        return 4 + (cpu.*instruction)(data);
      } else {
        return 4 + (cpu.*instruction)((highByte << 8) | lowByte);
      }
    }
};

struct AbsX {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto baseAddr = (highByte << 8) | lowByte;
      auto addr = baseAddr + cpu.getX();
      bool pageCrossed = (baseAddr & 0xFF00) != (addr & 0xFF00);
      if constexpr(m == IsLoad){
        auto data = cpu.load(addr);
        return 4 + pageCrossed + (cpu.*instruction)(data);
      } else {
        return 5 + (cpu.*instruction)(addr);
      }
    }
};

struct AbsY {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto baseAddr = (highByte << 8) | lowByte;
      auto addr = baseAddr + cpu.getY();
      bool pageCrossed = (baseAddr & 0xFF00) != (addr & 0xFF00);
      if constexpr(m == IsLoad) {
        auto data = cpu.load(addr);
        return 4 + pageCrossed + (cpu.*instruction)(data);
      } else {
        return 5 + (cpu.*instruction)(addr);
      }
    }
};

struct IndX {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto pageByte = cpu.load(cpu.nextByte());
      auto wrappedLow = (pageByte + cpu.getX()) & 0xFF;
      auto lowByte = cpu.load(wrappedLow);
      auto highByte = cpu.load(wrappedLow+1);
      auto addr = (highByte << 8) | lowByte;
      if constexpr(m == IsLoad) {
        auto data = cpu.load(addr);
        return 6 + (cpu.*instruction)(data);
      } else {
        return 6 + (cpu.*instruction)(addr);
      }
    }
};

struct IndY {
    template <MemoryAction m,typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto pageByte = cpu.load(cpu.nextByte());
      auto lowByte = cpu.load(pageByte);
      auto highByte = cpu.load(pageByte+1);
      auto baseAddr = (highByte << 8) | lowByte;
      auto addr = baseAddr + cpu.getY();
      bool pageCrossed = (baseAddr & 0xFF00) != (addr & 0xFF00);
      if constexpr(m == IsLoad) {
        auto data = cpu.load(addr);
        return 5 + pageCrossed + (cpu.*instruction)(data);
      } else {
        return 6 + (cpu.*instruction)(addr);
      }
    }
};

struct Accumulator {
    template <MemoryAction m, typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      return 2 + (cpu.*instruction)(0);
    }
};

struct Relative {
    template <MemoryAction m, typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      int8_t offset = static_cast<int8_t>(cpu.load(cpu.nextByte()));
      return 2 + (cpu.*instruction)(offset);
    }
};

struct Indirect {
    template <MemoryAction m, typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto ptrAddr = (highByte << 8) | lowByte;
      auto targetLow = cpu.load(ptrAddr);
      auto targetHigh = cpu.load(ptrAddr + 1);
      auto targetAddr = (targetHigh << 8) | targetLow;
      return 5 + (cpu.*instruction)(targetAddr);
    }
};
