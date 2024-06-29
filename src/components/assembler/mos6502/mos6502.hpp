#include "components/assembler/utils/asm_utils.hpp"
#include <string>
#include <string_view>
#include <cstdint>

namespace assembler {
    namespace mos6502 {
        struct OpType {
            constexpr Operation(const uint8_t opType, const uint8_t opCode, std::string_view& opPattern) : val_(val),
                code_(opCode), opPattern_(opPattern){}
            private:
                uint8_t type_;
                uint8_t code_;
                std::string_view& opPattern_;
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

        #define IMPL Operation(0, opCode, OpTypeEncoding::implicit);
        #define ACC Operation(1, opCode, OpTypeEncoding::accumulator);
        #define IMM Operation(2, opCode, OpTypeEncoding::immediate);
        #define ZP Operation(3, opCode, OpTypeEncoding::zeroPage);
        #define ZPX Operation(4, opCode, OpTypeEncoding::zeropPageX);
        #define ZPY Operation(5, opCode, OpTypeEncoding::zeroPageY);
        #define IZX Operation(6, opCode, OpTypeEncoding::indirectX);
        #define IZY Operation(7, opCode, OpTypeEncoding::indirectY);
        #define ABS Operation(8, opCode, OpTypeEncoding::absolute);
        #define ABX Operation(9, opCode, OpTypeEncoding::absoluteX);
        #define ABY Operation(10, opCode, OpTypeEncoding::absoluteY);
        #define IND Operation(11, opCode, OpTypeEncoding::indirect);
        #define REL Operation(12, opCode, OpTypeEncoding::relative);

    };
};
