#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace Components {
	class RomException : public std::exception {
	public:
	  RomException(std::string message) : exception_msg_(message){};
	  virtual const char *what() const throw() { return exception_msg_.c_str(); }

	private:
	  std::string exception_msg_;
	};

    /**
    * Rom is a utility to load data from some binary file and store it
    * in an underlying vector
    **/
	template<typename Storage>
	class Rom {
	public:
	  // For testing only
	  Rom(const std::filesystem::path &rom_path);

	  Rom(){};

	  // Could be in utils idk
	  auto load_rom(const std::filesystem::path &path) -> bool;
	  auto size() -> size_t { return data_.size(); };
	  auto begin() -> typename std::vector<Storage>::iterator;
	  auto end() -> typename std::vector<Storage>::iterator;
	  auto at(size_t index) -> Storage;

	private:
	  std::vector<Storage> data_;
	};
};
