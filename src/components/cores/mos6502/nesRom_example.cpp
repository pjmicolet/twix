#include "nesRom.hpp"
#include <iostream>
#include <format>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <path_to_nes_rom>\n";
        return 1;
    }

    try {
        cores::mos6502::NesRom rom(argv[1]);

        std::cout << "=== NES ROM Information ===\n";
        std::cout << std::format("Format Version: {}\n", 
            rom.getVersion() == cores::mos6502::INesVersion::INES_1_0 ? "iNES 1.0" : "iNES 2.0");
        
        std::cout << std::format("Mapper Number: {}\n", rom.getMapperNumber());
        
        if (rom.getVersion() == cores::mos6502::INesVersion::INES_2_0) {
            std::cout << std::format("Submapper Number: {}\n", rom.getSubmapperNumber());
        }

        std::cout << std::format("PRG ROM Size: {} bytes ({} KB)\n", 
            rom.getPrgRomSize(), rom.getPrgRomSize() / 1024);
        std::cout << std::format("CHR ROM Size: {} bytes ({} KB)\n", 
            rom.getChrRomSize(), rom.getChrRomSize() / 1024);

        if (rom.getPrgRamSize() > 0) {
            std::cout << std::format("PRG RAM Size: {} bytes ({} KB)\n", 
                rom.getPrgRamSize(), rom.getPrgRamSize() / 1024);
        }

        if (rom.getChrRamSize() > 0) {
            std::cout << std::format("CHR RAM Size: {} bytes ({} KB)\n", 
                rom.getChrRamSize(), rom.getChrRamSize() / 1024);
        }

        std::cout << std::format("Mirroring: {}\n", 
            rom.getMirroring() == cores::mos6502::Mirroring::HORIZONTAL ? "Horizontal" :
            rom.getMirroring() == cores::mos6502::Mirroring::VERTICAL ? "Vertical" : "Four-Screen");

        std::cout << std::format("Has Trainer: {}\n", rom.hasTrainer() ? "Yes" : "No");
        std::cout << std::format("Battery-Backed RAM: {}\n", rom.hasBatteryBackedRAM() ? "Yes" : "No");

        auto prg_rom = rom.getPrgRom();
        if (!prg_rom.empty()) {
            std::cout << std::format("\nFirst 16 bytes of PRG ROM: ");
            for (size_t i = 0; i < std::min(size_t(16), prg_rom.size()); ++i) {
                std::cout << std::format("{:02X} ", prg_rom[i]);
            }
            std::cout << "\n";
        }

        auto chr_rom = rom.getChrRom();
        if (!chr_rom.empty()) {
            std::cout << std::format("\nFirst 16 bytes of CHR ROM: ");
            for (size_t i = 0; i < std::min(size_t(16), chr_rom.size()); ++i) {
                std::cout << std::format("{:02X} ", chr_rom[i]);
            }
            std::cout << "\n";
        }

        std::cout << "\n=== Memory Mapping Example ===\n";
        std::cout << "To map PRG ROM to MOS 6502 memory:\n";
        std::cout << "  auto prg_data = rom.getPrgRom();\n";
        std::cout << "  // Map to $8000-$FFFF for typical NES cartridge\n";
        std::cout << "  // For 16KB PRG ROM, mirror at $C000\n";
        std::cout << "  // For 32KB PRG ROM, map directly\n";

    } catch (const cores::mos6502::NesRomException& e) {
        std::cerr << "Error loading ROM: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
