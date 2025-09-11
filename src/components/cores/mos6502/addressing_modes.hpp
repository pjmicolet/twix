#pragma once
//  auto implicit(fp exec) -> void {
//    (this->*exec)(1);
//    R.PC += 2;
//  }
//
//  auto zp(fp exec) -> void {
//    auto data = mem_component.load(mem_component.load(R.PC+1));
//    (this->*exec)(data);
//  }
//
//  inline auto safeAdd(uint8_t a, uint8_t b) -> uint8_t {
//    uint16_t c = a;
//    return (c+b) & 0xFF;
//  }
//
//  inline auto twoByteAddrData() -> uint8_t {
//    auto lowByte = mem_component.load((R.PC + 1));
//    auto highByte = mem_component.load((R.PC + 2));
//    return mem_component.load((highByte << 8) | lowByte);
//  }
//
//
//  auto zpx(fp exec) -> void {
//    auto data = safeAdd(R.PC+1, R.X);
//    (this->*exec)(data);
//  }
//
//  auto absAddr(fp exec) -> void {
//    auto data = twoByteAddrData(); 
//    (this->*exec)(data);
//  }
//
//  auto immediate(fp exec) -> void {
//    uint16_t imm = mem_component.load(R.PC+1);
//    (this->*exec)(imm);
//    R.PC += 2;
//  }
//
struct ImmediateMode {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      uint8_t mem = cpu.load(cpu.nextByte());
      return 3 + (cpu.*instruction)(mem);
    }
};

struct ZPX {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      uint16_t mem = static_cast<uint16_t>(cpu.getX() + cpu.nextByte())&0xFF;
      uint8_t data = cpu.load(mem);
      return 3 + (cpu.*instruction)(data);
    }
};

struct ZP {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto page = cpu.load(cpu.nextByte());
      return 3 + (cpu.*instruction)(cpu.load(page));
    }
};

struct AbsAddress {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto data = cpu.load((highByte << 8) | lowByte);
      return 3 + (cpu.*instruction)(data);
    }
};

struct AbsX {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto addr = ((highByte << 8) | lowByte) + cpu.getX();
      auto data = cpu.load(addr);
      return 3 + (cpu.*instruction)(data);
    }
};

struct AbsY {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto lowByte = cpu.load(cpu.nextByte());
      auto highByte = cpu.load(cpu.nextByte());
      auto addr = ((highByte << 8) | lowByte) + cpu.getY();
      auto data = cpu.load(addr);
      return 3 + (cpu.*instruction)(data);
    }
};

struct IndX {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto pageByte = cpu.load(cpu.nextByte());
      auto wrappedLow = (pageByte + cpu.getX()) & 0xFF;
      auto lowByte = cpu.load(wrappedLow);
      auto highByte = cpu.load(wrappedLow+1);
      auto addr = ((highByte << 8) | lowByte) + cpu.getY();
      auto data = cpu.load(addr);
      return 3 + (cpu.*instruction)(data);
    }
};

struct IndY {
    template <typename CPU, typename Input>
    static auto execute(CPU& cpu, uint64_t(CPU::*instruction)(Input)) -> uint64_t {
      auto pageByte = cpu.load(cpu.nextByte());
      auto lowByte = cpu.load(pageByte);
      auto highByte = cpu.load(pageByte+1);
      auto addr = ((highByte << 8) | lowByte) + cpu.getY();
      auto data = cpu.load(addr);
      return 3 + (cpu.*instruction)(data);
    }
};
