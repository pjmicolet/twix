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
#define INST(AddrMode, Operation) AddrMode::execute(*this,&mos6502::Operation);
#define DEFINE_INST(OpCode, AddrMode, Operation) case(OpCode) : { INST(AddrMode, Operation) break ;};

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
		
//Logical
#include "logical.hpp"

//Shifts
		auto asl(uint8_t addr) -> void {
			addr <<= 2; 
		}
public:
		auto runCycle() -> void {
			switch(mem_component.load(R.PC)) {
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

