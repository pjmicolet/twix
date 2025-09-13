#pragma once
#include <cstdint>
#include "memory.hpp"
#include "registers.hpp"
#include <type_traits>
#include <print>
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
			std::print("{}\n", R);
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

    auto load(uint16_t address) -> uint8_t {
      return mem_component.load(address);
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
     uint16_t tmp = R.ACC + m;
     R.setC(tmp); // easiest way
     R.Status.O = willOverflow(R.ACC, m, tmp&0xFF);
     R.ACC = R.ACC + m + R.Status.C;
     R.setZ(R.ACC);
    return 0;
  }

//Memory ops
  auto ldx(uint16_t m) -> uint64_t {
    R.X = m;
    R.Status.N = (R.X & 0x80);
    R.setZ(R.X);
    return 2;
  }

  auto lda(uint16_t m) -> uint64_t {
    R.ACC = m;
    R.Status.N = (R.ACC & 0x80);
    R.setZ(R.ACC);
    return 2;
  }

  auto ldy(uint16_t m) -> uint64_t {
    R.Y = m;
    R.Status.N = (R.Y & 0x80);
    R.setZ(R.Y);
    return 2;
  }

  auto stx(uint16_t m) -> uint64_t {
    mem_component.store(m, R.X);
    return 2;
  }

  auto sta(uint16_t m) -> uint64_t {
    mem_component.store(m, R.ACC);
    return 2;
  }

  auto sty(uint16_t m) -> uint64_t {
    mem_component.store(m, R.Y);
    return 2;
  }
		
//Logical
#include "logical.hpp"

//Shifts
		auto asl(uint8_t addr) -> void {
			addr <<= 2; 
		}
public:
		auto runCycle() -> void {
			switch(mem_component.load(R.PC)) {
      //memory
        DEFINE_INST(0xA9, ImmediateMode, ldx)
        DEFINE_INST(0xA5, ZP, ldx)
        DEFINE_INST(0xB5, ZPX, ldx)
        DEFINE_INST(0xAD, AbsAddress, ldx)
        DEFINE_INST(0xBD, AbsX, ldx)
        DEFINE_INST(0xB9, AbsY, ldx)

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

        DEFINE_STORE_INST(0x86, ZP, stx);
        DEFINE_STORE_INST(0x96, ZPY, stx);
        DEFINE_STORE_INST(0x8E, AbsAddress, stx);

				DEFINE_INST(0x69, ImmediateMode, adc)
				DEFINE_INST(0x65, ZP, adc)
				DEFINE_INST(0x75, ZPX, adc)
				DEFINE_INST(0x6D, AbsAddress, adc)
				DEFINE_INST(0x7D, AbsX, adc)
				DEFINE_INST(0x79, AbsY, adc)
				DEFINE_INST(0x61, IndX, adc)
				DEFINE_INST(0x71, IndY, adc)
			};
      nextByte();
		};

	};
};

};

