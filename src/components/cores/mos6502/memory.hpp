#pragma once
#include <concepts>

namespace cores {
	template<typename T>
	concept MemoryComponent = requires(T t, uint16_t address, uint8_t data) {
		{t.load(address)} -> std::same_as<uint8_t>;
		{t.store(address, data)} -> std::same_as<void>;
	};

	struct testMem {
		auto load(uint16_t address) -> uint8_t {
			return 0;
		}

		auto store(uint16_t address, uint8_t data) -> void {
		}
	};
}
