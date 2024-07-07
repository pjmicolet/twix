#include <assembler/utils/asm_utils.hpp>
#include <string>
#include <string_view>
#include <cstdint>
#include <unordered_map>

namespace assembler {
    namespace mos6502 {
		using byte_type = char;

        struct OpType {
            constexpr OpType(const uint8_t opType, const uint8_t opCode, const std::string_view& opPattern) : 
                type_(opType), code_(opCode), opPattern_(opPattern){}
			auto getType() -> char {
				return code_;
			}
			auto match(std::string_view& v) -> bool {
				return utils::match(v,opPattern_);
			}
            private:
                uint8_t type_;
                uint8_t code_;
                const std::string_view& opPattern_;
        };

        struct OpTypeEncoding {
            static inline constexpr std::string_view implicit = "";
            static inline constexpr std::string_view accumulator = "A";
            static inline constexpr std::string_view immediate = "#@byte";
            static inline constexpr std::string_view zeroPage = "$@byte";
            static inline constexpr std::string_view zeroPageX = "$@byte,X";
            static inline constexpr std::string_view zeroPageY = "$@byte,Y";
            static inline constexpr std::string_view relative = "@byte";
            static inline constexpr std::string_view absolute = "$@byte@byte";
            static inline constexpr std::string_view absoluteX = "$@byte@byte,X";
            static inline constexpr std::string_view absoluteY = "$@byte@byte,Y";
            static inline constexpr std::string_view indirect = "($@byte@byte)";
            static inline constexpr std::string_view indirectX = "($@byte,X)";
            static inline constexpr std::string_view indirectY = "($@byte),Y";
        };

        #define IMPL(opCode) OpType{0, opCode, OpTypeEncoding::implicit}
        #define ACC(opCode) OpType{1, opCode, OpTypeEncoding::accumulator}
        #define IMM(opCode) OpType{2, opCode, OpTypeEncoding::immediate}
        #define ZP(opCode) OpType{3, opCode, OpTypeEncoding::zeroPage}
        #define ZPX(opCode) OpType{4, opCode, OpTypeEncoding::zeropPageX}
        #define ZPY(opCode) OpType{5, opCode, OpTypeEncoding::zeroPageY}
        #define IZX(opCode) OpType{6, opCode, OpTypeEncoding::indirectX}
        #define IZY(opCode) OpType{7, opCode, OpTypeEncoding::indirectY}
        #define ABS(opCode) OpType{8, opCode, OpTypeEncoding::absolute}
        #define ABX(opCode) OpType{9, opCode, OpTypeEncoding::absoluteX}
        #define ABY(opCode) OpType{10, opCode, OpTypeEncoding::absoluteY}
        #define IND(opCode) OpType{11, opCode, OpTypeEncoding::indirect}
        #define REL(opCode) OpType{12, opCode, OpTypeEncoding::relative}

		struct Inst {
			Inst(std::string name, std::vector<OpType> types) {}
			auto match(std::string operand) -> byte_type;
			private:
				std::string name_;
				std::vector<OpType> types;
		};

		#define INSTRUCTION(name, ...) { name, Inst{name, {__VA_ARGS__}} }

        struct mos6502Assembler {
			auto assemble(const std::vector<std::string>& source) -> std::vector<byte_type>;
			private:
				auto assemble(const std::string& inst, std::vector<byte_type>& result) -> void;
				std::unordered_map<std::string, Inst> insts = {
					INSTRUCTION("ADD", IMPL(0), ACC(2), IMM(3))	
				};
        };

        struct mos6502Disassembler {
        };
    };
};
