#pragma once

#include <filesystem>

namespace TMDBTools
{
	void unpacker(const std::filesystem::path& srcExe, const std::filesystem::path& srcData, const std::filesystem::path& dest);
}