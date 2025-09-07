#pragma once
#include <cstdint>
#include "memory.hpp"
#include <type_traits>
#include <print>

namespace cores {
	// I basically had a quick look at different emulators, but mainly higen and also some cpp video about fast dispatch.
	// A lot of times I've tried doing this by defining functions with the inputs and then storing state, this time I want to try something similar to higen where 
	// it's the addressing modes that apply a function
	// It's pretty easy on the 6502 as all arithmetic and logical functions are applied to a single register (accumulator).
	struct Registers {
		struct PStat {
			uint8_t C = 0;
			uint8_t Z = 0;
			uint8_t I = 0;
			uint8_t D = 0;
			uint8_t B = 0;
			uint8_t O = 0;
			uint8_t N = 0;
		};
		uint16_t PC = 0;
		uint8_t ACC = 0;
		uint8_t X = 0;
		uint8_t Y = 0;
		PStat Status;
	};
	template<typename Memory = cores::testMem> requires cores::MemoryComponent<Memory>
	struct mos6502 {

#define INST(AddrMode, Operation) AddrMode(&mos6502::Operation);
#define DEFINE_INST(OpCode, AddrMode, Operation) case(OpCode) : { INST(AddrMode, Operation) break ;};

		mos6502() {};
		mos6502(Memory& mem) : mem_component(mem) {
			static_assert(std::is_same_v<MemType, Memory&>, "We're good");
		};

		auto runCycle() -> void {
			switch(mem_component.load(R.PC)) {
				DEFINE_INST(0x69, immediate, adc)
			};
		};

		auto setPC(uint16_t pc) -> void {
			R.PC = pc;
		}

		auto showState() -> void {
			std::print("{}\n", R);
		}


private:
		using fp = void (mos6502::*)(uint16_t);
		using implicitFp = void (mos6502::*)();
		using onAddress = void (mos6502::*)(uint8_t&);

		using MemType = std::conditional_t<std::is_same_v<Memory, cores::testMem>, Memory, Memory&>;
		MemType& mem_component;

		Registers R;

// Addressing modes
		auto implicit(fp exec) -> void {
			(this->*exec)(1);
			R.PC += 2;
		}

		auto zp(fp exec) -> void {
			auto data = mem_component.load(mem_component.load(R.PC+1));
			(this->*exec)(data);
		}

		inline auto safeAdd(uint8_t a, uint8_t b) -> uint8_t {
			uint16_t c = a;
			return (c+b) & 0xFF;
		}

		inline auto twoByteAddrData() -> uint8_t {
			auto lowByte = mem_component.load((R.PC + 1));
			auto highByte = mem_component.load((R.PC + 2));
			return mem_component.load((highByte << 8) | lowByte);
		}

		auto willOverflow(uint8_t acc, uint8_t mem, uint16_t res) -> bool {
			return (acc^res) & (mem^res) & 0x80 != 0x00;
		}

		auto zpx(fp exec) -> void {
			auto data = safeAdd(R.PC+1, R.X);
			(this->*exec)(data);
		}

		auto absAddr(fp exec) -> void {
			auto data = twoByteAddrData(); 
			(this->*exec)(data);
		}

		auto immediate(fp exec) -> void {
			uint16_t imm = mem_component.load(R.PC+1);
			(this->*exec)(imm);
			R.PC += 2;
		}

//Arithmetic Operations
		
		auto adc(uint16_t m) -> void {
			uint16_t tmp = R.ACC + m;
			R.Status.C = tmp & 0x100; // easiest way
			R.Status.O = willOverflow(R.ACC, m, tmp&0xFF);
			R.ACC = R.ACC + m + R.Status.C;
		}
		
//Logical
		auto andop(uint16_t m) -> void {
			R.ACC = R.ACC & m;
		}

//Shifts
		auto asl(uint8_t addr) -> void {
			addr <<= 2; 
		}
	};

};

template<>
struct std::formatter<cores::Registers> {
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}
	
	auto format(const cores::Registers& r, std::format_context& ctx) const {
		return std::format_to(
			ctx.out(),
			"PC: {:x}, ACC: {:x}, X: {:x}, Y: {:x}, Status:[C:{:x}, Z:{:x}, I:{:x}, D:{:x}, B:{:x}, O:{:x}, N:{:x}]",
			r.PC,
			r.ACC,
			r.X, r.Y, r.Status.C, r.Status.Z, r.Status.I, r.Status.D,
			r.Status.B, r.Status.O, r.Status.N
		);
	}
};

