#pragma once

#include <cstdint>
#include <vector>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <span>

namespace cores {
namespace mos6502 {

class NesRomException : public std::runtime_error {
public:
    explicit NesRomException(const std::string& message) 
        : std::runtime_error(message) {}
};

enum class INesVersion {
    INES_1_0,
    INES_2_0,
    UNKNOWN
};

enum class Mirroring {
    HORIZONTAL = 0,
    VERTICAL = 1,
    FOUR_SCREEN = 2
};

enum class ConsoleType {
    NES_FAMICOM = 0,
    VS_SYSTEM = 1,
    PLAYCHOICE_10 = 2,
    EXTENDED = 3
};

struct INesHeader {
    uint8_t magic[4];
    uint8_t prg_rom_size;
    uint8_t chr_rom_size;
    uint8_t flags6;
    uint8_t flags7;
    uint8_t flags8;
    uint8_t flags9;
    uint8_t flags10;
    uint8_t padding[5];

    auto isValid() const -> bool {
        return magic[0] == 'N' && magic[1] == 'E' && 
               magic[2] == 'S' && magic[3] == 0x1A;
    }

    auto getVersion() const -> INesVersion {
        if ((flags7 & 0x0C) == 0x08) {
            return INesVersion::INES_2_0;
        } else if ((flags7 & 0x0C) == 0x00) {
            return INesVersion::INES_1_0;
        }
        return INesVersion::UNKNOWN;
    }

    auto hasTrainer() const -> bool {
        return (flags6 & 0x04) != 0;
    }

    auto hasBatteryBackedRAM() const -> bool {
        return (flags6 & 0x02) != 0;
    }

    auto getMirroring() const -> Mirroring {
        if (flags6 & 0x08) {
            return Mirroring::FOUR_SCREEN;
        }
        return (flags6 & 0x01) ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;
    }

    auto getMapperNumber() const -> uint16_t {
        if (getVersion() == INesVersion::INES_2_0) {
            return ((flags8 & 0x0F) << 8) | (flags7 & 0xF0) | ((flags6 & 0xF0) >> 4);
        } else {
            return (flags7 & 0xF0) | ((flags6 & 0xF0) >> 4);
        }
    }

    auto getSubmapperNumber() const -> uint8_t {
        if (getVersion() == INesVersion::INES_2_0) {
            return (flags8 & 0xF0) >> 4;
        }
        return 0;
    }

    auto getPrgRomSize() const -> size_t {
        if (getVersion() == INesVersion::INES_2_0) {
            uint16_t msb = (flags9 & 0x0F) << 8;
            return (msb | prg_rom_size) * 16384;
        } else {
            return static_cast<size_t>(prg_rom_size) * 16384;
        }
    }

    auto getChrRomSize() const -> size_t {
        if (getVersion() == INesVersion::INES_2_0) {
            uint16_t msb = (flags9 & 0xF0) << 4;
            return (msb | chr_rom_size) * 8192;
        } else {
            return static_cast<size_t>(chr_rom_size) * 8192;
        }
    }

    auto getPrgRamSize() const -> size_t {
        if (getVersion() == INesVersion::INES_2_0) {
            uint8_t shift = flags10 & 0x0F;
            if (shift == 0) return 0;
            return 64 << shift;
        } else {
            if (flags8 == 0) return 8192;
            return static_cast<size_t>(flags8) * 8192;
        }
    }

    auto getChrRamSize() const -> size_t {
        if (getVersion() == INesVersion::INES_2_0) {
            uint8_t shift = (flags10 & 0xF0) >> 4;
            if (shift == 0) return 0;
            return 64 << shift;
        }
        return 0;
    }

    auto getConsoleType() const -> ConsoleType {
        return static_cast<ConsoleType>(flags7 & 0x03);
    }

    auto hasPrgNVRAM() const -> bool {
        if (getVersion() == INesVersion::INES_2_0) {
            return (flags10 & 0x0F) != 0;
        }
        return hasBatteryBackedRAM();
    }
};

class NesRom {
public:
    NesRom() = default;

    explicit NesRom(const std::filesystem::path& rom_path) {
        if (!loadRom(rom_path)) {
            throw NesRomException("Failed to load ROM from: " + rom_path.string());
        }
    }

    auto loadRom(const std::filesystem::path& path) -> bool {
        if (!std::filesystem::exists(path)) {
            throw NesRomException("ROM file does not exist: " + path.string());
        }

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            return false;
        }

        auto file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (file_size < 16) {
            throw NesRomException("File too small to be a valid iNES ROM");
        }

        file.read(reinterpret_cast<char*>(&header_), sizeof(INesHeader));
        
        if (!header_.isValid()) {
            throw NesRomException("Invalid iNES header magic number");
        }

        version_ = header_.getVersion();
        if (version_ == INesVersion::UNKNOWN) {
            throw NesRomException("Unknown iNES format version");
        }

        size_t trainer_size = header_.hasTrainer() ? 512 : 0;
        size_t prg_rom_size = header_.getPrgRomSize();
        size_t chr_rom_size = header_.getChrRomSize();

        size_t expected_size = 16 + trainer_size + prg_rom_size + chr_rom_size;
        if (static_cast<size_t>(file_size) < expected_size) {
            throw NesRomException("File size mismatch with header specifications");
        }

        size_t total_data_size = trainer_size + prg_rom_size + chr_rom_size;
        data_.resize(total_data_size);

        if (!file.read(reinterpret_cast<char*>(data_.data()), total_data_size)) {
            throw NesRomException("Failed to read ROM data");
        }

        trainer_offset_ = 0;
        prg_rom_offset_ = trainer_size;
        chr_rom_offset_ = trainer_size + prg_rom_size;

        return true;
    }

    auto getHeader() const -> const INesHeader& {
        return header_;
    }

    auto getVersion() const -> INesVersion {
        return version_;
    }

    auto hasTrainer() const -> bool {
        return header_.hasTrainer();
    }

    auto getTrainer() const -> std::span<const uint8_t> {
        if (!hasTrainer()) {
            return std::span<const uint8_t>();
        }
        return std::span<const uint8_t>(data_.data() + trainer_offset_, 512);
    }

    auto getPrgRom() const -> std::span<const uint8_t> {
        size_t size = header_.getPrgRomSize();
        if (size == 0) {
            return std::span<const uint8_t>();
        }
        return std::span<const uint8_t>(data_.data() + prg_rom_offset_, size);
    }

    auto getChrRom() const -> std::span<const uint8_t> {
        size_t size = header_.getChrRomSize();
        if (size == 0) {
            return std::span<const uint8_t>();
        }
        return std::span<const uint8_t>(data_.data() + chr_rom_offset_, size);
    }

    auto getPrgRomMutable() -> std::span<uint8_t> {
        size_t size = header_.getPrgRomSize();
        if (size == 0) {
            return std::span<uint8_t>();
        }
        return std::span<uint8_t>(data_.data() + prg_rom_offset_, size);
    }

    auto getChrRomMutable() -> std::span<uint8_t> {
        size_t size = header_.getChrRomSize();
        if (size == 0) {
            return std::span<uint8_t>();
        }
        return std::span<uint8_t>(data_.data() + chr_rom_offset_, size);
    }

    auto getMapperNumber() const -> uint16_t {
        return header_.getMapperNumber();
    }

    auto getSubmapperNumber() const -> uint8_t {
        return header_.getSubmapperNumber();
    }

    auto getMirroring() const -> Mirroring {
        return header_.getMirroring();
    }

    auto hasBatteryBackedRAM() const -> bool {
        return header_.hasBatteryBackedRAM();
    }

    auto getPrgRomSize() const -> size_t {
        return header_.getPrgRomSize();
    }

    auto getChrRomSize() const -> size_t {
        return header_.getChrRomSize();
    }

    auto getPrgRamSize() const -> size_t {
        return header_.getPrgRamSize();
    }

    auto getChrRamSize() const -> size_t {
        return header_.getChrRamSize();
    }

    auto getConsoleType() const -> ConsoleType {
        return header_.getConsoleType();
    }

    auto isValid() const -> bool {
        return header_.isValid() && version_ != INesVersion::UNKNOWN;
    }

    auto getRawData() const -> const std::vector<uint8_t>& {
        return data_;
    }

private:
    INesHeader header_{};
    INesVersion version_{INesVersion::UNKNOWN};
    std::vector<uint8_t> data_;
    size_t trainer_offset_{0};
    size_t prg_rom_offset_{0};
    size_t chr_rom_offset_{0};
};

}
}
