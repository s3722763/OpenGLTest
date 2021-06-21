#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

namespace Utilities {
	template<typename T>
	inline void read_buffer_to_type(std::vector<unsigned char>* bytes, size_t offset, T* result) {
		size_t size = sizeof(T);
		unsigned char data[sizeof(T)]{};

		for (size_t i = 0; i < size; i++) {
			data[size - i - 1] = bytes->at(offset + (size - i - 1));
		}

		memcpy(result, data, sizeof(T));
	}
}