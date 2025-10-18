#pragma once
#include <cstdint>
#include "memory.hpp"
#include "registers.hpp"
#include <type_traits>
#if __cpp_lib_print >= 202207L
#include <print>
#else
#include <iostream>
#include <format>
#endif
#include "addressing_modes.hpp"

namespace cores {
namespace mos6502 {
  // I basically had a quick look at different emulators, but mainly higen and also some cpp video about fast dispatch.
  // A lot of times I've tried doing this by defining functions with the inputs and then storing state, this time I want to try something similar to higen where
  // it's the addressing modes that apply a function
  // It's pretty easy on the 6502 as all arithmetic and logical functions are applied to a single register (accumulator).
template<typename Memory = cores::testMem> requires cores::MemoryComponent<Memory>
struct mos6502 {
#define INST(AddrMode, MemOp, Operation) AddrMode::execute<MemOp>(*this,&mos6502::Operation);
#define DEFINE_INST(OpCode, AddrMode, Operation) case(OpCode) : { INST(AddrMode, MemoryAction::IsLoad, Operation) break ;};
#define DEFINE_STORE_INST(OpCode, AddrMode, Operation) case(OpCode) : { INST(AddrMode, MemoryAction::IsStore, Operation) break ;};

    mos6502() {};
    mos6502(Memory& mem) : mem_component(mem) {
      static_assert(std::is_same_v<MemType, Memory&>, "We're good");
    };

    auto setPC(uint16_t pc) -> void {
      R.PC = pc;
    }

    auto clearState() -> void {
      R.reset();
    }

    auto showState() -> void {
#if __cpp_lib_print >= 202207L
      std::print("{}\n", R);
#else
      std::cout << std::format("{}\n", R);
#endif
    }

    auto nextByte() -> uint16_t {
      R.PC++;
      return R.PC;
    }

    auto setX(uint8_t data) -> void {
      R.X = data;
    }

    auto setY(uint8_t data) -> void {
      R.Y = data;
    }

    auto getX() -> uint8_t {
      return R.X;
    }

    auto getY() -> uint8_t {
      return R.Y;
    }

    auto getAcc() -> uint8_t {
      return R.ACC;
    }

    auto getCarry() -> uint8_t {
      return R.Status.C;
    }

    auto getZero() -> uint8_t {
      return R.Status.Z;
    }

    auto getInt() -> uint8_t {
      return R.Status.I;
    }

    auto getDig() -> uint8_t {
      return R.Status.D;
    }

    auto load(uint16_t address) -> uint8_t {
      return mem_component.load(address);
    }

    auto pushStack(uint8_t value) -> void {
      mem_component.store(0x0100 + R.SP, value);
      R.SP--;
    }

    auto popStack() -> uint8_t {
      R.SP++;
      return mem_component.load(0x0100 + R.SP);
    }

    auto getStatusByte() -> uint8_t {
      return (R.Status.N << 7) | (R.Status.O << 6) | (1 << 5) | (R.Status.B << 4) |
             (R.Status.D << 3) | (R.Status.I << 2) | (R.Status.Z << 1) | R.Status.C;
    }

    auto setStatusByte(uint8_t status) -> void {
      R.Status.N = (status >> 7) & 1;
      R.Status.O = (status >> 6) & 1;
      R.Status.B = (status >> 4) & 1;
      R.Status.D = (status >> 3) & 1;
      R.Status.I = (status >> 2) & 1;
      R.Status.Z = (status >> 1) & 1;
      R.Status.C = status & 1;
    }

private:
    using fp = void (mos6502::*)(uint16_t);
    using implicitFp = void (mos6502::*)();
    using onAddress = void (mos6502::*)(uint8_t&);

    using MemType = std::conditional_t<std::is_same_v<Memory, cores::testMem>, Memory, Memory&>;
    MemType& mem_component;

    Registers R;

    auto willOverflow(uint8_t acc, uint8_t mem, uint16_t res) -> bool {
      return ((acc^res) & (mem^res) & 0x80) != 0x00;
    }

public:
//Arithmetic Operations
  auto adc(uint16_t m) -> uint64_t {
     uint16_t tmp = R.ACC + m + R.Status.C;
     uint8_t oldC = R.Status.C;
     R.Status.C = (tmp & 0x100 )!= 0; // easiest way
     R.Status.O = willOverflow(R.ACC, m, tmp&0xFF);
     R.ACC = R.ACC + m + oldC;
     R.setZ(R.ACC);
    return 0;
  }

  auto sbc(uint16_t m) -> uint64_t {
    //Just read into it
    adc(~m);
    return 0;
  }

//Status Flags Ops
  auto clc(uint16_t m) -> uint64_t {
    R.Status.C = 0;
    return 0;
  }

  auto cld(uint16_t m) -> uint64_t {
    R.Status.D = 0;
    return 0;
  }

  auto cli(uint16_t m) -> uint64_t {
    R.Status.I = 0;
    return 0;
  }

  auto clv(uint16_t m) -> uint64_t {
    R.Status.O = 0;
    return 0;
  }

  auto sec(uint16_t m) -> uint64_t {
    R.Status.C = 1;
    return 0;
  }

  auto sed(uint16_t m) -> uint64_t {
    R.Status.D = 1;
    return 0;
  }

  auto sei(uint16_t m) -> uint64_t {
    R.Status.I = 1;
    return 0;
  }

//Memory ops
  auto ldx(uint16_t m) -> uint64_t {
    R.X = m;
    R.Status.N = (R.X & 0x80);
    R.setZ(R.X);
    return 0;
  }

  auto lda(uint16_t m) -> uint64_t {
    R.ACC = m;
    R.Status.N = (R.ACC & 0x80);
    R.setZ(R.ACC);
    return 0;
  }

  auto ldy(uint16_t m) -> uint64_t {
    R.Y = m;
    R.Status.N = (R.Y & 0x80);
    R.setZ(R.Y);
    return 0;
  }

  auto stx(uint16_t m) -> uint64_t {
    mem_component.store(m, R.X);
    return 0;
  }

  auto sta(uint16_t m) -> uint64_t {
    mem_component.store(m, R.ACC);
    return 0;
  }

  auto sty(uint16_t m) -> uint64_t {
    mem_component.store(m, R.Y);
    return 0;
  }

//Logical

  auto and_op(uint8_t data) -> uint64_t {
     R.ACC &= data;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto bit(uint8_t data) -> uint64_t {
     auto res = R.ACC & data;
     R.Status.Z = res == 0;
     R.Status.N = data & 0x80;
     R.Status.O = data & 0x40;
     return 0;
  }

  auto eor_op(uint8_t data) -> uint64_t {
     R.ACC ^= data;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto ora_op(uint8_t data) -> uint64_t {
     R.ACC |= data;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

//Comparisons
  auto cmp(uint8_t data) -> uint64_t {
     uint16_t result = R.ACC - data;
     R.Status.C = R.ACC >= data;
     R.Status.Z = R.ACC == data;
     R.Status.N = (result & 0x80) != 0;
     return 0;
  }

  auto cpx(uint8_t data) -> uint64_t {
     uint16_t result = R.X - data;
     R.Status.C = R.X >= data;
     R.Status.Z = R.X == data;
     R.Status.N = (result & 0x80) != 0;
     return 0;
  }

  auto cpy(uint8_t data) -> uint64_t {
     uint16_t result = R.Y - data;
     R.Status.C = R.Y >= data;
     R.Status.Z = R.Y == data;
     R.Status.N = (result & 0x80) != 0;
     return 0;
  }

//Increment/Decrement
  auto inc(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     value++;
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = value & 0x80;
     return 2;
  }

  auto inx(uint16_t dummy) -> uint64_t {
     R.X++;
     R.Status.Z = R.X == 0;
     R.Status.N = R.X & 0x80;
     return 0;
  }

  auto iny(uint16_t dummy) -> uint64_t {
     R.Y++;
     R.Status.Z = R.Y == 0;
     R.Status.N = R.Y & 0x80;
     return 0;
  }

  auto dec(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     value--;
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = value & 0x80;
     return 2;
  }

  auto dex(uint16_t dummy) -> uint64_t {
     R.X--;
     R.Status.Z = R.X == 0;
     R.Status.N = R.X & 0x80;
     return 0;
  }

  auto dey(uint16_t dummy) -> uint64_t {
     R.Y--;
     R.Status.Z = R.Y == 0;
     R.Status.N = R.Y & 0x80;
     return 0;
  }

//Shifts and Rotates
  auto asl_acc(uint16_t dummy) -> uint64_t {
     R.Status.C = (R.ACC & 0x80) != 0;
     R.ACC <<= 1;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto asl_mem(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     R.Status.C = (value & 0x80) != 0;
     value <<= 1;
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = value & 0x80;
     return 2;
  }

  auto lsr_acc(uint16_t dummy) -> uint64_t {
     R.Status.C = R.ACC & 0x01;
     R.ACC >>= 1;
     R.Status.Z = R.ACC == 0;
     R.Status.N = 0;
     return 0;
  }

  auto lsr_mem(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     R.Status.C = value & 0x01;
     value >>= 1;
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = 0;
     return 2;
  }

  auto rol_acc(uint16_t dummy) -> uint64_t {
     uint8_t oldCarry = R.Status.C;
     R.Status.C = (R.ACC & 0x80) != 0;
     R.ACC = (R.ACC << 1) | oldCarry;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto rol_mem(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     uint8_t oldCarry = R.Status.C;
     R.Status.C = (value & 0x80) != 0;
     value = (value << 1) | oldCarry;
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = value & 0x80;
     return 2;
  }

  auto ror_acc(uint16_t dummy) -> uint64_t {
     uint8_t oldCarry = R.Status.C;
     R.Status.C = R.ACC & 0x01;
     R.ACC = (R.ACC >> 1) | (oldCarry << 7);
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto ror_mem(uint16_t addr) -> uint64_t {
     uint8_t value = mem_component.load(addr);
     uint8_t oldCarry = R.Status.C;
     R.Status.C = value & 0x01;
     value = (value >> 1) | (oldCarry << 7);
     mem_component.store(addr, value);
     R.Status.Z = value == 0;
     R.Status.N = value & 0x80;
     return 2;
  }

//Branches
  auto bcc(int8_t offset) -> uint64_t {
     if (R.Status.C == 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bcs(int8_t offset) -> uint64_t {
     if (R.Status.C != 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto beq(int8_t offset) -> uint64_t {
     if (R.Status.Z != 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bmi(int8_t offset) -> uint64_t {
     if (R.Status.N != 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bne(int8_t offset) -> uint64_t {
     if (R.Status.Z == 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bpl(int8_t offset) -> uint64_t {
     if (R.Status.N == 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bvc(int8_t offset) -> uint64_t {
     if (R.Status.O == 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

  auto bvs(int8_t offset) -> uint64_t {
     if (R.Status.O != 0) {
       R.PC += offset;
       return 1;
     }
     return 0;
  }

//Transfers
  auto tax(uint16_t dummy) -> uint64_t {
     R.X = R.ACC;
     R.Status.Z = R.X == 0;
     R.Status.N = R.X & 0x80;
     return 0;
  }

  auto tay(uint16_t dummy) -> uint64_t {
     R.Y = R.ACC;
     R.Status.Z = R.Y == 0;
     R.Status.N = R.Y & 0x80;
     return 0;
  }

  auto tsx(uint16_t dummy) -> uint64_t {
     R.X = R.SP;
     R.Status.Z = R.X == 0;
     R.Status.N = R.X & 0x80;
     return 0;
  }

  auto txa(uint16_t dummy) -> uint64_t {
     R.ACC = R.X;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

  auto txs(uint16_t dummy) -> uint64_t {
     R.SP = R.X;
     return 0;
  }

  auto tya(uint16_t dummy) -> uint64_t {
     R.ACC = R.Y;
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 0;
  }

//Stack Operations
  auto pha(uint16_t dummy) -> uint64_t {
     pushStack(R.ACC);
     return 1;
  }

  auto php(uint16_t dummy) -> uint64_t {
     pushStack(getStatusByte() | 0x10);
     return 1;
  }

  auto pla(uint16_t dummy) -> uint64_t {
     R.ACC = popStack();
     R.Status.Z = R.ACC == 0;
     R.Status.N = R.ACC & 0x80;
     return 2;
  }

  auto plp(uint16_t dummy) -> uint64_t {
     setStatusByte(popStack());
     return 2;
  }

//Jumps and Calls
  auto jmp(uint16_t addr) -> uint64_t {
     R.PC = addr - 1;
     return 0;
  }

  auto jsr(uint16_t addr) -> uint64_t {
     uint16_t returnAddr = R.PC;
     pushStack((returnAddr >> 8) & 0xFF);
     pushStack(returnAddr & 0xFF);
     R.PC = addr - 1;
     return 2;
  }

  auto rts(uint16_t dummy) -> uint64_t {
     uint8_t low = popStack();
     uint8_t high = popStack();
     R.PC = (high << 8) | low;
     return 4;
  }

  auto rti(uint16_t dummy) -> uint64_t {
     setStatusByte(popStack());
     uint8_t low = popStack();
     uint8_t high = popStack();
     R.PC = ((high << 8) | low) - 1;
     return 4;
  }

  auto brk(uint16_t dummy) -> uint64_t {
     R.PC++;
     pushStack((R.PC >> 8) & 0xFF);
     pushStack(R.PC & 0xFF);
     pushStack(getStatusByte() | 0x10);
     R.Status.I = 1;
     uint8_t low = mem_component.load(0xFFFE);
     uint8_t high = mem_component.load(0xFFFF);
     R.PC = ((high << 8) | low) - 1;
     return 5;
  }

//NOP
  auto nop(uint16_t dummy) -> uint64_t {
     return 0;
  }

public:
    auto runCycle() -> void {
      switch(mem_component.load(R.PC)) {
        //memory
        DEFINE_INST(0xA9, ImmediateMode, lda)
        DEFINE_INST(0xA5, ZP, lda)
        DEFINE_INST(0xB5, ZPX, lda)
        DEFINE_INST(0xAD, AbsAddress, lda)
        DEFINE_INST(0xBD, AbsX, lda)
        DEFINE_INST(0xB9, AbsY, lda)

        DEFINE_INST(0xA2, ImmediateMode, ldx)
        DEFINE_INST(0xA6, ZP, ldx)
        DEFINE_INST(0xB6, ZPY, ldx)
        DEFINE_INST(0xAE, AbsAddress, ldx)
        DEFINE_INST(0xBE, AbsY, ldx)

        DEFINE_INST(0xA0, ImmediateMode, ldy)
        DEFINE_INST(0xA4, ZP, ldy)
        DEFINE_INST(0xB4, ZPX, ldy)
        DEFINE_INST(0xAC, AbsAddress, ldy)
        DEFINE_INST(0xBC, AbsX, ldy)

        DEFINE_STORE_INST(0x85, ZP, sta);
        DEFINE_STORE_INST(0x95, ZPX, sta);
        DEFINE_STORE_INST(0x8D, AbsAddress, sta);
        DEFINE_STORE_INST(0x9D, AbsX, sta);
        DEFINE_STORE_INST(0x99, AbsY, sta);
        DEFINE_STORE_INST(0x81, IndX, sta);
        DEFINE_STORE_INST(0x91, IndY, sta);

        DEFINE_STORE_INST(0x86, ZP, stx);
        DEFINE_STORE_INST(0x96, ZPY, stx);
        DEFINE_STORE_INST(0x8E, AbsAddress, stx);

        DEFINE_STORE_INST(0x84, ZP, sty);
        DEFINE_STORE_INST(0x92, ZPY, sty);
        DEFINE_STORE_INST(0x8C, AbsAddress, sty);

//arithmetic
        DEFINE_INST(0x69, ImmediateMode, adc)
        DEFINE_INST(0x65, ZP, adc)
        DEFINE_INST(0x75, ZPX, adc)
        DEFINE_INST(0x6D, AbsAddress, adc)
        DEFINE_INST(0x7D, AbsX, adc)
        DEFINE_INST(0x79, AbsY, adc)
        DEFINE_INST(0x61, IndX, adc)
        DEFINE_INST(0x71, IndY, adc)

        DEFINE_INST(0xE9, ImmediateMode, sbc)
        DEFINE_INST(0xE5, ZP, sbc)
        DEFINE_INST(0xF5, ZPX, sbc)
        DEFINE_INST(0xED, AbsAddress, sbc)
        DEFINE_INST(0xFD, AbsX, sbc)
        DEFINE_INST(0xF9, AbsY, sbc)
        DEFINE_INST(0xE1, IndX, sbc)
        DEFINE_INST(0xF1, IndY, sbc)

        DEFINE_INST(0x18, Implied, clc)
        DEFINE_INST(0xD8, Implied, cld)
        DEFINE_INST(0x58, Implied, cli)
        DEFINE_INST(0xB8, Implied, clv)

        DEFINE_INST(0x38, Implied, sec)
        DEFINE_INST(0xF8, Implied, sed)
        DEFINE_INST(0x78, Implied, sei)
//Logical

        DEFINE_INST(0x29, ImmediateMode, and_op)
        DEFINE_INST(0x25, ZP, and_op)
        DEFINE_INST(0x35, ZPX, and_op)
        DEFINE_INST(0x2D, AbsAddress, and_op)
        DEFINE_INST(0x3D, AbsX, and_op)
        DEFINE_INST(0x39, AbsY, and_op)
        DEFINE_INST(0x21, IndX, and_op)
        DEFINE_INST(0x31, IndY, and_op)

        DEFINE_INST(0x49, ImmediateMode, eor_op)
        DEFINE_INST(0x45, ZP, eor_op)
        DEFINE_INST(0x55, ZPX, eor_op)
        DEFINE_INST(0x4D, AbsAddress, eor_op)
        DEFINE_INST(0x5D, AbsX, eor_op)
        DEFINE_INST(0x59, AbsY, eor_op)
        DEFINE_INST(0x41, IndX, eor_op)
        DEFINE_INST(0x51, IndY, eor_op)

        DEFINE_INST(0x09, ImmediateMode, ora_op)
        DEFINE_INST(0x05, ZP, ora_op)
        DEFINE_INST(0x15, ZPX, ora_op)
        DEFINE_INST(0x0D, AbsAddress, ora_op)
        DEFINE_INST(0x1D, AbsX, ora_op)
        DEFINE_INST(0x19, AbsY, ora_op)
        DEFINE_INST(0x01, IndX, ora_op)
        DEFINE_INST(0x11, IndY, ora_op)

        DEFINE_INST(0x24, ZP, bit)
        DEFINE_INST(0x2C, AbsAddress, bit)

//Comparisons
        DEFINE_INST(0xC9, ImmediateMode, cmp)
        DEFINE_INST(0xC5, ZP, cmp)
        DEFINE_INST(0xD5, ZPX, cmp)
        DEFINE_INST(0xCD, AbsAddress, cmp)
        DEFINE_INST(0xDD, AbsX, cmp)
        DEFINE_INST(0xD9, AbsY, cmp)
        DEFINE_INST(0xC1, IndX, cmp)
        DEFINE_INST(0xD1, IndY, cmp)

        DEFINE_INST(0xE0, ImmediateMode, cpx)
        DEFINE_INST(0xE4, ZP, cpx)
        DEFINE_INST(0xEC, AbsAddress, cpx)

        DEFINE_INST(0xC0, ImmediateMode, cpy)
        DEFINE_INST(0xC4, ZP, cpy)
        DEFINE_INST(0xCC, AbsAddress, cpy)

//Increment/Decrement
        DEFINE_STORE_INST(0xE6, ZP, inc)
        DEFINE_STORE_INST(0xF6, ZPX, inc)
        DEFINE_STORE_INST(0xEE, AbsAddress, inc)
        DEFINE_STORE_INST(0xFE, AbsX, inc)

        DEFINE_INST(0xE8, Implied, inx)
        DEFINE_INST(0xC8, Implied, iny)

        DEFINE_STORE_INST(0xC6, ZP, dec)
        DEFINE_STORE_INST(0xD6, ZPX, dec)
        DEFINE_STORE_INST(0xCE, AbsAddress, dec)
        DEFINE_STORE_INST(0xDE, AbsX, dec)

        DEFINE_INST(0xCA, Implied, dex)
        DEFINE_INST(0x88, Implied, dey)

//Shifts and Rotates
        DEFINE_INST(0x0A, Accumulator, asl_acc)
        DEFINE_STORE_INST(0x06, ZP, asl_mem)
        DEFINE_STORE_INST(0x16, ZPX, asl_mem)
        DEFINE_STORE_INST(0x0E, AbsAddress, asl_mem)
        DEFINE_STORE_INST(0x1E, AbsX, asl_mem)

        DEFINE_INST(0x4A, Accumulator, lsr_acc)
        DEFINE_STORE_INST(0x46, ZP, lsr_mem)
        DEFINE_STORE_INST(0x56, ZPX, lsr_mem)
        DEFINE_STORE_INST(0x4E, AbsAddress, lsr_mem)
        DEFINE_STORE_INST(0x5E, AbsX, lsr_mem)

        DEFINE_INST(0x2A, Accumulator, rol_acc)
        DEFINE_STORE_INST(0x26, ZP, rol_mem)
        DEFINE_STORE_INST(0x36, ZPX, rol_mem)
        DEFINE_STORE_INST(0x2E, AbsAddress, rol_mem)
        DEFINE_STORE_INST(0x3E, AbsX, rol_mem)

        DEFINE_INST(0x6A, Accumulator, ror_acc)
        DEFINE_STORE_INST(0x66, ZP, ror_mem)
        DEFINE_STORE_INST(0x76, ZPX, ror_mem)
        DEFINE_STORE_INST(0x6E, AbsAddress, ror_mem)
        DEFINE_STORE_INST(0x7E, AbsX, ror_mem)

//Branches
        DEFINE_INST(0x90, Relative, bcc)
        DEFINE_INST(0xB0, Relative, bcs)
        DEFINE_INST(0xF0, Relative, beq)
        DEFINE_INST(0x30, Relative, bmi)
        DEFINE_INST(0xD0, Relative, bne)
        DEFINE_INST(0x10, Relative, bpl)
        DEFINE_INST(0x50, Relative, bvc)
        DEFINE_INST(0x70, Relative, bvs)

//Transfers
        DEFINE_INST(0xAA, Implied, tax)
        DEFINE_INST(0xA8, Implied, tay)
        DEFINE_INST(0xBA, Implied, tsx)
        DEFINE_INST(0x8A, Implied, txa)
        DEFINE_INST(0x9A, Implied, txs)
        DEFINE_INST(0x98, Implied, tya)

//Stack Operations
        DEFINE_INST(0x48, Implied, pha)
        DEFINE_INST(0x08, Implied, php)
        DEFINE_INST(0x68, Implied, pla)
        DEFINE_INST(0x28, Implied, plp)

//Jumps and Calls
        DEFINE_STORE_INST(0x4C, AbsAddress, jmp)
        DEFINE_STORE_INST(0x6C, Indirect, jmp)
        DEFINE_STORE_INST(0x20, AbsAddress, jsr)
        DEFINE_INST(0x60, Implied, rts)
        DEFINE_INST(0x40, Implied, rti)
        DEFINE_INST(0x00, Implied, brk)

//NOP
        DEFINE_INST(0xEA, Implied, nop)
    };
      nextByte();
    };

  };
};

};

