#pragma once
#include <cstdint>

namespace cores {
namespace mos6502 {

// Cycle validation table for all 256 MOS 6502 opcodes
// Format: [opcode] = {min_cycles, max_cycles}
// min_cycles: base cycle count
// max_cycles: cycle count with page crossing (if applicable)
// Source: http://www.6502.org/users/obelisk/6502/reference.html

struct CycleRange {
    uint8_t min;
    uint8_t max;
};

constexpr CycleRange CYCLE_TABLE[256] = {
    // 0x00-0x0F
    {7, 7},   // 0x00 BRK
    {6, 6},   // 0x01 ORA (Indirect,X)
    {0, 0},   // 0x02 Invalid
    {0, 0},   // 0x03 Invalid
    {0, 0},   // 0x04 Invalid
    {3, 3},   // 0x05 ORA Zero Page
    {5, 5},   // 0x06 ASL Zero Page
    {0, 0},   // 0x07 Invalid
    {3, 3},   // 0x08 PHP
    {2, 2},   // 0x09 ORA Immediate
    {2, 2},   // 0x0A ASL Accumulator
    {0, 0},   // 0x0B Invalid
    {0, 0},   // 0x0C Invalid
    {4, 4},   // 0x0D ORA Absolute
    {6, 6},   // 0x0E ASL Absolute
    {0, 0},   // 0x0F Invalid

    // 0x10-0x1F
    {2, 4},   // 0x10 BPL (+1 if branch, +2 if page cross)
    {5, 6},   // 0x11 ORA (Indirect),Y (+1 if page cross)
    {0, 0},   // 0x12 Invalid
    {0, 0},   // 0x13 Invalid
    {0, 0},   // 0x14 Invalid
    {4, 4},   // 0x15 ORA Zero Page,X
    {6, 6},   // 0x16 ASL Zero Page,X
    {0, 0},   // 0x17 Invalid
    {2, 2},   // 0x18 CLC
    {4, 5},   // 0x19 ORA Absolute,Y (+1 if page cross)
    {0, 0},   // 0x1A Invalid
    {0, 0},   // 0x1B Invalid
    {0, 0},   // 0x1C Invalid
    {4, 5},   // 0x1D ORA Absolute,X (+1 if page cross)
    {7, 7},   // 0x1E ASL Absolute,X
    {0, 0},   // 0x1F Invalid

    // 0x20-0x2F
    {6, 6},   // 0x20 JSR
    {6, 6},   // 0x21 AND (Indirect,X)
    {0, 0},   // 0x22 Invalid
    {0, 0},   // 0x23 Invalid
    {3, 3},   // 0x24 BIT Zero Page
    {3, 3},   // 0x25 AND Zero Page
    {5, 5},   // 0x26 ROL Zero Page
    {0, 0},   // 0x27 Invalid
    {4, 4},   // 0x28 PLP
    {2, 2},   // 0x29 AND Immediate
    {2, 2},   // 0x2A ROL Accumulator
    {0, 0},   // 0x2B Invalid
    {4, 4},   // 0x2C BIT Absolute
    {4, 4},   // 0x2D AND Absolute
    {6, 6},   // 0x2E ROL Absolute
    {0, 0},   // 0x2F Invalid

    // 0x30-0x3F
    {2, 4},   // 0x30 BMI (+1 if branch, +2 if page cross)
    {5, 6},   // 0x31 AND (Indirect),Y (+1 if page cross)
    {0, 0},   // 0x32 Invalid
    {0, 0},   // 0x33 Invalid
    {0, 0},   // 0x34 Invalid
    {4, 4},   // 0x35 AND Zero Page,X
    {6, 6},   // 0x36 ROL Zero Page,X
    {0, 0},   // 0x37 Invalid
    {2, 2},   // 0x38 SEC
    {4, 5},   // 0x39 AND Absolute,Y (+1 if page cross)
    {0, 0},   // 0x3A Invalid
    {0, 0},   // 0x3B Invalid
    {0, 0},   // 0x3C Invalid
    {4, 5},   // 0x3D AND Absolute,X (+1 if page cross)
    {7, 7},   // 0x3E ROL Absolute,X
    {0, 0},   // 0x3F Invalid

    // 0x40-0x4F
    {6, 6},   // 0x40 RTI
    {6, 6},   // 0x41 EOR (Indirect,X)
    {0, 0},   // 0x42 Invalid
    {0, 0},   // 0x43 Invalid
    {0, 0},   // 0x44 Invalid
    {3, 3},   // 0x45 EOR Zero Page
    {5, 5},   // 0x46 LSR Zero Page
    {0, 0},   // 0x47 Invalid
    {3, 3},   // 0x48 PHA
    {2, 2},   // 0x49 EOR Immediate
    {2, 2},   // 0x4A LSR Accumulator
    {0, 0},   // 0x4B Invalid
    {3, 3},   // 0x4C JMP Absolute
    {4, 4},   // 0x4D EOR Absolute
    {6, 6},   // 0x4E LSR Absolute
    {0, 0},   // 0x4F Invalid

    // 0x50-0x5F
    {2, 4},   // 0x50 BVC (+1 if branch, +2 if page cross)
    {5, 6},   // 0x51 EOR (Indirect),Y (+1 if page cross)
    {0, 0},   // 0x52 Invalid
    {0, 0},   // 0x53 Invalid
    {0, 0},   // 0x54 Invalid
    {4, 4},   // 0x55 EOR Zero Page,X
    {6, 6},   // 0x56 LSR Zero Page,X
    {0, 0},   // 0x57 Invalid
    {2, 2},   // 0x58 CLI
    {4, 5},   // 0x59 EOR Absolute,Y (+1 if page cross)
    {0, 0},   // 0x5A Invalid
    {0, 0},   // 0x5B Invalid
    {0, 0},   // 0x5C Invalid
    {4, 5},   // 0x5D EOR Absolute,X (+1 if page cross)
    {7, 7},   // 0x5E LSR Absolute,X
    {0, 0},   // 0x5F Invalid

    // 0x60-0x6F
    {6, 6},   // 0x60 RTS
    {6, 6},   // 0x61 ADC (Indirect,X)
    {0, 0},   // 0x62 Invalid
    {0, 0},   // 0x63 Invalid
    {0, 0},   // 0x64 Invalid
    {3, 3},   // 0x65 ADC Zero Page
    {5, 5},   // 0x66 ROR Zero Page
    {0, 0},   // 0x67 Invalid
    {4, 4},   // 0x68 PLA
    {2, 2},   // 0x69 ADC Immediate
    {2, 2},   // 0x6A ROR Accumulator
    {0, 0},   // 0x6B Invalid
    {5, 5},   // 0x6C JMP Indirect
    {4, 4},   // 0x6D ADC Absolute
    {6, 6},   // 0x6E ROR Absolute
    {0, 0},   // 0x6F Invalid

    // 0x70-0x7F
    {2, 4},   // 0x70 BVS (+1 if branch, +2 if page cross)
    {5, 6},   // 0x71 ADC (Indirect),Y (+1 if page cross)
    {0, 0},   // 0x72 Invalid
    {0, 0},   // 0x73 Invalid
    {0, 0},   // 0x74 Invalid
    {4, 4},   // 0x75 ADC Zero Page,X
    {6, 6},   // 0x76 ROR Zero Page,X
    {0, 0},   // 0x77 Invalid
    {2, 2},   // 0x78 SEI
    {4, 5},   // 0x79 ADC Absolute,Y (+1 if page cross)
    {0, 0},   // 0x7A Invalid
    {0, 0},   // 0x7B Invalid
    {0, 0},   // 0x7C Invalid
    {4, 5},   // 0x7D ADC Absolute,X (+1 if page cross)
    {7, 7},   // 0x7E ROR Absolute,X
    {0, 0},   // 0x7F Invalid

    // 0x80-0x8F
    {0, 0},   // 0x80 Invalid
    {6, 6},   // 0x81 STA (Indirect,X)
    {0, 0},   // 0x82 Invalid
    {0, 0},   // 0x83 Invalid
    {3, 3},   // 0x84 STY Zero Page
    {3, 3},   // 0x85 STA Zero Page
    {3, 3},   // 0x86 STX Zero Page
    {0, 0},   // 0x87 Invalid
    {2, 2},   // 0x88 DEY
    {0, 0},   // 0x89 Invalid
    {2, 2},   // 0x8A TXA
    {0, 0},   // 0x8B Invalid
    {4, 4},   // 0x8C STY Absolute
    {4, 4},   // 0x8D STA Absolute
    {4, 4},   // 0x8E STX Absolute
    {0, 0},   // 0x8F Invalid

    // 0x90-0x9F
    {2, 4},   // 0x90 BCC (+1 if branch, +2 if page cross)
    {6, 6},   // 0x91 STA (Indirect),Y
    {0, 0},   // 0x92 Invalid
    {0, 0},   // 0x93 Invalid
    {4, 4},   // 0x94 STY Zero Page,X
    {4, 4},   // 0x95 STA Zero Page,X
    {4, 4},   // 0x96 STX Zero Page,Y
    {0, 0},   // 0x97 Invalid
    {2, 2},   // 0x98 TYA
    {5, 5},   // 0x99 STA Absolute,Y
    {2, 2},   // 0x9A TXS
    {0, 0},   // 0x9B Invalid
    {0, 0},   // 0x9C Invalid
    {5, 5},   // 0x9D STA Absolute,X
    {0, 0},   // 0x9E Invalid
    {0, 0},   // 0x9F Invalid

    // 0xA0-0xAF
    {2, 2},   // 0xA0 LDY Immediate
    {6, 6},   // 0xA1 LDA (Indirect,X)
    {2, 2},   // 0xA2 LDX Immediate
    {0, 0},   // 0xA3 Invalid
    {3, 3},   // 0xA4 LDY Zero Page
    {3, 3},   // 0xA5 LDA Zero Page
    {3, 3},   // 0xA6 LDX Zero Page
    {0, 0},   // 0xA7 Invalid
    {2, 2},   // 0xA8 TAY
    {2, 2},   // 0xA9 LDA Immediate
    {2, 2},   // 0xAA TAX
    {0, 0},   // 0xAB Invalid
    {4, 4},   // 0xAC LDY Absolute
    {4, 4},   // 0xAD LDA Absolute
    {4, 4},   // 0xAE LDX Absolute
    {0, 0},   // 0xAF Invalid

    // 0xB0-0xBF
    {2, 4},   // 0xB0 BCS (+1 if branch, +2 if page cross)
    {5, 6},   // 0xB1 LDA (Indirect),Y (+1 if page cross)
    {0, 0},   // 0xB2 Invalid
    {0, 0},   // 0xB3 Invalid
    {4, 4},   // 0xB4 LDY Zero Page,X
    {4, 4},   // 0xB5 LDA Zero Page,X
    {4, 4},   // 0xB6 LDX Zero Page,Y
    {0, 0},   // 0xB7 Invalid
    {2, 2},   // 0xB8 CLV
    {4, 5},   // 0xB9 LDA Absolute,Y (+1 if page cross)
    {2, 2},   // 0xBA TSX
    {0, 0},   // 0xBB Invalid
    {4, 5},   // 0xBC LDY Absolute,X (+1 if page cross)
    {4, 5},   // 0xBD LDA Absolute,X (+1 if page cross)
    {4, 5},   // 0xBE LDX Absolute,Y (+1 if page cross)
    {0, 0},   // 0xBF Invalid

    // 0xC0-0xCF
    {2, 2},   // 0xC0 CPY Immediate
    {6, 6},   // 0xC1 CMP (Indirect,X)
    {0, 0},   // 0xC2 Invalid
    {0, 0},   // 0xC3 Invalid
    {3, 3},   // 0xC4 CPY Zero Page
    {3, 3},   // 0xC5 CMP Zero Page
    {5, 5},   // 0xC6 DEC Zero Page
    {0, 0},   // 0xC7 Invalid
    {2, 2},   // 0xC8 INY
    {2, 2},   // 0xC9 CMP Immediate
    {2, 2},   // 0xCA DEX
    {0, 0},   // 0xCB Invalid
    {4, 4},   // 0xCC CPY Absolute
    {4, 4},   // 0xCD CMP Absolute
    {6, 6},   // 0xCE DEC Absolute
    {0, 0},   // 0xCF Invalid

    // 0xD0-0xDF
    {2, 4},   // 0xD0 BNE (+1 if branch, +2 if page cross)
    {5, 6},   // 0xD1 CMP (Indirect),Y (+1 if page cross)
    {0, 0},   // 0xD2 Invalid
    {0, 0},   // 0xD3 Invalid
    {0, 0},   // 0xD4 Invalid
    {4, 4},   // 0xD5 CMP Zero Page,X
    {6, 6},   // 0xD6 DEC Zero Page,X
    {0, 0},   // 0xD7 Invalid
    {2, 2},   // 0xD8 CLD
    {4, 5},   // 0xD9 CMP Absolute,Y (+1 if page cross)
    {0, 0},   // 0xDA Invalid
    {0, 0},   // 0xDB Invalid
    {0, 0},   // 0xDC Invalid
    {4, 5},   // 0xDD CMP Absolute,X (+1 if page cross)
    {7, 7},   // 0xDE DEC Absolute,X
    {0, 0},   // 0xDF Invalid

    // 0xE0-0xEF
    {2, 2},   // 0xE0 CPX Immediate
    {6, 6},   // 0xE1 SBC (Indirect,X)
    {0, 0},   // 0xE2 Invalid
    {0, 0},   // 0xE3 Invalid
    {3, 3},   // 0xE4 CPX Zero Page
    {3, 3},   // 0xE5 SBC Zero Page
    {5, 5},   // 0xE6 INC Zero Page
    {0, 0},   // 0xE7 Invalid
    {2, 2},   // 0xE8 INX
    {2, 2},   // 0xE9 SBC Immediate
    {2, 2},   // 0xEA NOP
    {0, 0},   // 0xEB Invalid
    {4, 4},   // 0xEC CPX Absolute
    {4, 4},   // 0xED SBC Absolute
    {6, 6},   // 0xEE INC Absolute
    {0, 0},   // 0xEF Invalid

    // 0xF0-0xFF
    {2, 4},   // 0xF0 BEQ (+1 if branch, +2 if page cross)
    {5, 6},   // 0xF1 SBC (Indirect),Y (+1 if page cross)
    {0, 0},   // 0xF2 Invalid
    {0, 0},   // 0xF3 Invalid
    {0, 0},   // 0xF4 Invalid
    {4, 4},   // 0xF5 SBC Zero Page,X
    {6, 6},   // 0xF6 INC Zero Page,X
    {0, 0},   // 0xF7 Invalid
    {2, 2},   // 0xF8 SED
    {4, 5},   // 0xF9 SBC Absolute,Y (+1 if page cross)
    {0, 0},   // 0xFA Invalid
    {0, 0},   // 0xFB Invalid
    {0, 0},   // 0xFC Invalid
    {4, 5},   // 0xFD SBC Absolute,X (+1 if page cross)
    {7, 7},   // 0xFE INC Absolute,X
    {0, 0}    // 0xFF Invalid
};

} // namespace mos6502
} // namespace cores
