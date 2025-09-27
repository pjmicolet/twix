#pragma once
#include <print>

namespace cores {
namespace mos6502 {
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

    auto setZ(uint8_t val) -> void {
      Status.Z = val == 0 ? 0x1: 0x0; 
    }
    auto setC(uint16_t val) -> void {
      Status.C = val; 
    }
    auto reset() -> void {
       PC = 0;
       ACC = 0;
       X = 0;
       Y = 0;
 			 Status.C = 0;
			 Status.Z = 0;
			 Status.I = 0;
			 Status.D = 0;
			 Status.B = 0;
			 Status.O = 0;
			 Status.N = 0;
    }
	};
}
}

template<>
struct std::formatter<cores::mos6502::Registers> {
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}
	
	auto format(const cores::mos6502::Registers& r, std::format_context& ctx) const {
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
