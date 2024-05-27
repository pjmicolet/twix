#include "rom.hpp"
#include <fstream>

namespace Components {
	template<typename Storage>
	Rom<Storage>::Rom(const std::filesystem::path &rom_path) {
	  if (std::filesystem::exists(rom_path)) {
		if (!load_rom(rom_path)) {
		  throw RomException("Found rom but couldn't load");
		}
	  } else {
		throw RomException("Invalid rom path");
	  }
	}

	template<typename Storage>
	auto Rom<Storage>::load_rom(const std::filesystem::path &path) -> bool {

	  std::ifstream rom(path, std::ios::binary | std::ios::ate);
	  if (!rom)
		return false;

	  auto end = rom.tellg();
	  rom.seekg(0, std::ios::beg);

	  auto size = std::size_t(end - rom.tellg());
	  if (size == 0)
		return false;

	  data_.resize(size);
	  if (!rom.read((char *)data_.data(), static_cast<std::streamsize>(data_.size())))
		return false;

	  return true;
	}

	template<typename Storage>
	auto Rom<Storage>::begin() -> typename std::vector<Storage>::iterator {
	  return data_.begin();
	}

	template<typename Storage>
	auto Rom<Storage>::end() -> typename std::vector<Storage>::iterator {
	  return data_.end();
	}

	template<typename Storage>
	auto Rom<Storage>::at(size_t index) -> Storage {
	  return data_[index];
	}
};
