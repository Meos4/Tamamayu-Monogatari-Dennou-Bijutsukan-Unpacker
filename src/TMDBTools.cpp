#include "TMDBTools.hpp"

#include "TMDBExe.hpp"
#include "Types.hpp"

#include "fmt/format.h"

#include <array>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace TMDBTools
{
	static constexpr auto sectorSize{ 2048u };

	static constexpr auto 
		data001Filename{ "DATA.001" },
		psxExeFilename{ "SLPM_803.25" };

	struct DslLOC
	{
		u8 minutes;
		u8 seconds;
		u8 sectors;
		u8 track;
	};

	struct FileInfo
	{
		DslLOC position;
		u32 nbSectors;
		u32 size;
	};

	static s32 DsPosToInt(DslLOC* p)
	{
		s32 v0, v1, a0, a1, a2;
		v1 = p->minutes;
		a2 = p->seconds;
		a1 = v1 >> 4;
		v0 = a1 << 2;
		v0 += a1;
		v0 <<= 1;
		v1 &= 0xF;
		v0 += v1;
		a1 = v0 << 4;
		a1 -= v0;
		a1 <<= 2;
		v1 = a2 >> 4;
		v0 = v1 << 2;
		v0 += v1;
		v0 <<= 1;
		a2 &= 0xF;
		v0 += a2;
		a1 += v0;
		v1 = a1 << 2;
		v1 += a1;
		v0 = v1 << 4;
		a1 = p->sectors;
		v0 -= v1;
		a0 = a1 >> 4;
		v1 = a0 << 2;
		v1 += a0;
		v1 <<= 1;
		a1 &= 0xF;
		v1 += a1;
		v0 += v1;
		return v0 - 0x96;
	}

	static DslLOC* DsIntToPos(s32 i, DslLOC* p)
	{
		s32 v1, a0, a1, a2, a3, t0, t1, t3, hi;
		v1 = 0x1B4E81B5;
		a0 = i + 0x96;
		hi = (static_cast<u64>(a0) * v1) >> 32;
		a1 = 0x88888889;
		v1 = hi;
		a3 = v1 >> 3;
		v1 = a0 >> 31;
		a3 -= v1;
		hi = (static_cast<u64>(a3) * a1) >> 32;
		t1 = 0x66666667;
		a1 = a3 << 2;
		a1 += a3;
		v1 = a1 << 4;
		a2 = hi;
		v1 -= a1;
		a0 -= v1;
		hi = (static_cast<u64>(a0) * t1) >> 32;
		v1 = a3 >> 31;
		t0 = a2 + a3;
		t0 >>= 5;
		t0 -= v1;
		v1 = t0 << 4;
		v1 -= t0;
		a1 = hi;
		v1 <<= 2;
		a3 -= v1;
		hi = (static_cast<u64>(a3) * t1) >> 32;
		v1 = a0 >> 31;
		a1 >>= 2;
		a1 -= v1;
		a2 = a1 << 4;
		v1 = a1 << 2;
		v1 += a1;
		v1 <<= 1;
		a0 -= v1;
		t3 = hi;
		a2 += a0;
		v1 = a3 >> 31;
		hi = (static_cast<u64>(t0) * t1) >> 32;
		p->sectors = static_cast<u8>(a2);
		a0 = t3 >> 2;
		a0 -= v1;
		a1 = a0 << 4;
		v1 = a0 << 2;
		v1 += a0;
		v1 <<= 1;
		a3 -= v1;
		a1 += a3;
		v1 = t0 >> 31;
		p->seconds = static_cast<u8>(a1);
		t1 = hi;
		a0 = t1 >> 2;
		a0 -= v1;
		a1 = a0 << 4;
		v1 = a0 << 2;
		v1 += a0;
		v1 <<= 1;
		t0 -= v1;
		a1 += t0;
		p->minutes = static_cast<u8>(a1);
		return p;
	}

	void unpacker(const std::filesystem::path& srcExe, const std::filesystem::path& srcData, const std::filesystem::path& dest)
	{
		const std::filesystem::path data001Path{ fmt::format("{}/{}", srcData.string(), data001Filename) };

		if (!std::filesystem::is_regular_file(data001Path))
		{
			throw std::runtime_error{ fmt::format("Can't find \"{}\" in \"{}\"", data001Filename, srcData.string()) };
		}

		const std::filesystem::path psxExePath{ fmt::format("{}/{}", srcExe.string(), psxExeFilename) };
		
		if (!std::filesystem::is_regular_file(psxExePath))
		{
			throw std::runtime_error{ fmt::format("Can't find \"{}\" in \"{}\"", psxExeFilename, srcExe.string()) };
		}

		if (!TMDBExe::isValid(psxExePath))
		{
			throw std::runtime_error{ fmt::format("\"{}\" is invalid", psxExeFilename) };
		}

		std::ifstream
			data001{ data001Path, std::ifstream::binary },
			executable{ psxExePath, std::ifstream::binary };

		static constexpr auto nbFiles{ static_cast<u32>(TMDBExe::data001FilesPath.size()) };
		std::array<FileInfo, nbFiles> filesInfo;

		executable.seekg(0x00031790);
		executable.read((char*)filesInfo.data(), nbFiles * sizeof(FileInfo));
		const auto data001Sector{ DsPosToInt(&filesInfo[0].position) };

		const auto maxFileSizeElem{ std::max_element(filesInfo.begin(), filesInfo.end(),
			[](const FileInfo& a, const FileInfo& b)
			{
				return a.size < b.size;
			})};

		std::filesystem::create_directories(dest);

		fmt::print("Unpacking files...\n");

		std::vector<char> buffer(maxFileSizeElem->size);
		auto* const bufferPtr{ buffer.data() };

		for (u32 i{}; i < nbFiles; ++i)
		{
			const std::filesystem::path filePath{ fmt::format("{}/{}", dest.string(), TMDBExe::data001FilesPath[i]) };
			const auto fileSize{ filesInfo[i].size };
			std::filesystem::create_directories(filePath.parent_path());

			data001.seekg((DsPosToInt(&filesInfo[i].position) - data001Sector) * sectorSize);
			data001.read(bufferPtr, fileSize);

			std::ofstream file{ filePath, std::ofstream::binary };
			file.write(bufferPtr, fileSize);
		}

		fmt::print("{} Files unpacked\n", nbFiles);
	}
}