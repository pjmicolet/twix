#include <assembler/mos6502/mos6502.hpp>

namespace assembler { namespace mos6502 {
	auto mos6502Assembler::assemble(const std::vector<std::string>& source) -> std::vector<byte_type> {
		std::vector<byte_type> result;
		for(auto& inst : source) {
			assemble(inst, result);
		}
		return result;
	}

	auto mos6502Assembler::assemble(const std::string& inst, std::vector<byte_type>& result) -> void {
	}

};};
