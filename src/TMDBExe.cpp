#include "TMDBExe.hpp"

#include <fstream>
#include <type_traits>

namespace TMDBExe
{
	bool isValid(const std::filesystem::path& path)
	{
		static constexpr std::array<u8, 8> pattern{ 0x44, 0x41, 0x54, 0x41, 0x2E, 0x30, 0x30, 0x31 };

		std::ifstream file{ path, std::ifstream::binary };
		std::decay_t<decltype(pattern)> bufferPattern;
		file.seekg(0x00001692);
		file.read((char*)bufferPattern.data(), bufferPattern.size());

		return pattern == bufferPattern;
	}
}