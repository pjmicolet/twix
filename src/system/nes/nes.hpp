#pragma once
#include <cores/mos6502/cpu.hpp>
#include <array>

template<typename Mapper>
struct NesRAM {
  NesRAM(cores::mos6502::NesRom& rom) : rom_(rom), mapper_(rom) {
    // 2KB internal RAM
    internal_ram_.fill(0);
    // 8KB PRG RAM for cartridge (used by some mappers)
    prg_ram_.fill(0);
  }

  auto load(uint16_t address) -> uint8_t {
    // $0000-$1FFF: 2KB internal RAM (mirrored 4 times)
    if(address <= 0x1FFF) {
      return internal_ram_[address & 0x7FF];
    }
    // $2000-$3FFF: PPU registers (mirrored every 8 bytes)
    // TODO: Implement PPU register reads
    else if(address <= 0x3FFF) {
      // For now, return 0 (PPU not implemented yet)
      return 0;
    }
    // $4000-$4017: APU and I/O registers
    // TODO: Implement APU/IO register reads
    else if(address <= 0x4017) {
      return 0;
    }
    // $4018-$401F: APU and I/O functionality (normally disabled)
    else if(address <= 0x401F) {
      return 0;
    }
    // $4020-$FFFF: Cartridge space (handled by mapper)
    else {
      return mapper_.read(address);
    }
  }

  auto store(uint16_t address, uint8_t data) -> void {
    // $0000-$1FFF: 2KB internal RAM (mirrored 4 times)
    if(address <= 0x1FFF) {
      internal_ram_[address & 0x7FF] = data;
    }
    // $2000-$3FFF: PPU registers (mirrored every 8 bytes)
    // TODO: Implement PPU register writes
    else if(address <= 0x3FFF) {
      // PPU not implemented yet
    }
    // $4000-$4017: APU and I/O registers
    // TODO: Implement APU/IO register writes
    else if(address <= 0x4017) {
      // APU/IO not implemented yet
    }
    // $4018-$401F: APU and I/O functionality (normally disabled)
    else if(address <= 0x401F) {
      // Normally disabled
    }
    // $4020-$FFFF: Cartridge space (handled by mapper)
    else {
      mapper_.write(address, data);
    }
  }

  std::array<uint8_t, 0x800> internal_ram_;  // 2KB internal RAM
  std::array<uint8_t, 0x2000> prg_ram_;      // 8KB PRG RAM (for cartridge)
  cores::mos6502::NesRom& rom_;
  Mapper mapper_;
};

// Mapper 0 (NROM) - No bank switching
// PRG ROM size: 16 KB or 32 KB
// $8000-$BFFF: First 16 KB of ROM
// $C000-$FFFF: Last 16 KB of ROM (or mirror of $8000-$BFFF if only 16KB)
struct Mapper0 {
  explicit Mapper0(cores::mos6502::NesRom& rom) : rom_(rom) {
    prg_rom_size_ = rom.getPrgRomSize();
    // NROM has either 16KB or 32KB PRG ROM
    is_16kb_ = (prg_rom_size_ == 16384);
  }

  auto read(uint16_t address) -> uint8_t {
    // $6000-$7FFF: Family Basic PRG RAM (optional, not commonly used)
    if(address >= 0x6000 && address <= 0x7FFF) {
      // Not implemented for now
      return 0;
    }
    // $8000-$FFFF: PRG ROM
    else if(address >= 0x8000) {
      uint16_t rom_address = address - 0x8000;
      // If 16KB ROM, mirror the upper 16KB to lower 16KB
      if(is_16kb_) {
        rom_address &= 0x3FFF;  // Wrap to 16KB
      }
      return rom_.loadFromPrg(rom_address);
    }
    return 0;
  }

  auto write(uint16_t address, uint8_t data) -> void {
    // $6000-$7FFF: Family Basic PRG RAM (optional)
    if(address >= 0x6000 && address <= 0x7FFF) {
      // Not implemented for now
    }
    // $8000-$FFFF: PRG ROM is read-only, writes are ignored
  }

private:
  cores::mos6502::NesRom& rom_;
  size_t prg_rom_size_;
  bool is_16kb_;
};

// Mapper 1 (MMC1) - Bank switching with shift register
// PRG ROM: Up to 256 KB (switchable 16 KB banks)
// PRG RAM: 8 KB (optional)
// Control register at $8000-$9FFF
// CHR bank 0 at $A000-$BFFF
// CHR bank 1 at $C000-$DFFF
// PRG bank at $E000-$FFFF
struct Mapper1 {
  explicit Mapper1(cores::mos6502::NesRom& rom) : rom_(rom) {
    prg_rom_size_ = rom.getPrgRomSize();
    prg_bank_count_ = prg_rom_size_ / 16384;  // Number of 16KB banks
    reset();
  }

  auto reset() -> void {
    shift_register_ = 0x10;  // Bit 4 set indicates empty
    write_count_ = 0;
    control_ = 0x0C;  // Default: $8000 and $C000 swappable, $C000 fixed to last bank
    chr_bank_0_ = 0;
    chr_bank_1_ = 0;
    prg_bank_ = 0;
    updateBanks();
  }

  auto read(uint16_t address) -> uint8_t {
    // $6000-$7FFF: 8 KB PRG RAM
    if(address >= 0x6000 && address <= 0x7FFF) {
      return prg_ram_[address - 0x6000];
    }
    // $8000-$BFFF: 16 KB PRG ROM bank (switchable or fixed)
    else if(address >= 0x8000 && address <= 0xBFFF) {
      uint16_t offset = address - 0x8000;
      return rom_.loadFromPrg(prg_bank_low_ * 16384 + offset);
    }
    // $C000-$FFFF: 16 KB PRG ROM bank (switchable or fixed)
    else if(address >= 0xC000) {
      uint16_t offset = address - 0xC000;
      return rom_.loadFromPrg(prg_bank_high_ * 16384 + offset);
    }
    return 0;
  }

  auto write(uint16_t address, uint8_t data) -> void {
    // $6000-$7FFF: 8 KB PRG RAM
    if(address >= 0x6000 && address <= 0x7FFF) {
      prg_ram_[address - 0x6000] = data;
    }
    // $8000-$FFFF: Mapper control registers (shift register)
    else if(address >= 0x8000) {
      // Bit 7 set = reset shift register
      if(data & 0x80) {
        shift_register_ = 0x10;
        write_count_ = 0;
        control_ |= 0x0C;  // Set bits 2-3
        updateBanks();
        return;
      }

      // Write bit 0 to shift register
      shift_register_ >>= 1;
      shift_register_ |= (data & 1) << 4;
      write_count_++;

      // After 5 writes, update the appropriate register
      if(write_count_ == 5) {
        uint8_t register_value = shift_register_ & 0x1F;
        
        // Determine which register to write to based on address
        if(address >= 0x8000 && address <= 0x9FFF) {
          // Control register
          control_ = register_value;
        }
        else if(address >= 0xA000 && address <= 0xBFFF) {
          // CHR bank 0
          chr_bank_0_ = register_value;
        }
        else if(address >= 0xC000 && address <= 0xDFFF) {
          // CHR bank 1
          chr_bank_1_ = register_value;
        }
        else if(address >= 0xE000) {
          // PRG bank
          prg_bank_ = register_value & 0x0F;  // Only lower 4 bits used
        }

        updateBanks();
        shift_register_ = 0x10;
        write_count_ = 0;
      }
    }
  }

private:
  auto updateBanks() -> void {
    uint8_t prg_mode = (control_ >> 2) & 0x03;
    
    switch(prg_mode) {
      case 0:
      case 1:
        // 32 KB mode: switch 32 KB at $8000, ignoring low bit of bank number
        prg_bank_low_ = (prg_bank_ & 0x0E) % prg_bank_count_;
        prg_bank_high_ = ((prg_bank_ & 0x0E) + 1) % prg_bank_count_;
        break;
      case 2:
        // Fix first bank at $8000, switch 16 KB bank at $C000
        prg_bank_low_ = 0;
        prg_bank_high_ = prg_bank_ % prg_bank_count_;
        break;
      case 3:
        // Fix last bank at $C000, switch 16 KB bank at $8000
        prg_bank_low_ = prg_bank_ % prg_bank_count_;
        prg_bank_high_ = (prg_bank_count_ - 1);
        break;
    }
  }

  cores::mos6502::NesRom& rom_;
  std::array<uint8_t, 0x2000> prg_ram_;  // 8KB PRG RAM
  
  size_t prg_rom_size_;
  size_t prg_bank_count_;
  
  // MMC1 state
  uint8_t shift_register_;
  uint8_t write_count_;
  uint8_t control_;
  uint8_t chr_bank_0_;
  uint8_t chr_bank_1_;
  uint8_t prg_bank_;
  
  // Current bank mappings
  size_t prg_bank_low_;
  size_t prg_bank_high_;
};
