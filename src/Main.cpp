#include "TMDBTools.hpp"

#include "fmt/format.h"

#include <filesystem>
#include <iostream>
#include <limits>
#include <stdexcept>

int main(int argc, char** argv)
{
	try
	{
		fmt::print("Tamamayu Monogatari - Dennou Bijutsukan Unpacker v1.0.0 by Meos\n\n");

		if (argc < 2)
		{
			const auto currentPath{ std::filesystem::current_path() };
			TMDBTools::unpacker(currentPath, currentPath, currentPath);
		}
		else
		{
			if (argc > 3)
			{
				TMDBTools::unpacker(argv[1], argv[2], argv[3]);
			}
			else
			{
				throw std::runtime_error
				{ 
					"Invalid arguments\n" 
					"Arguments: [SLPM_803.25 path] [DATA.001 path] [Unpacked files path]\n"
				};
			}
		}
	}
	catch (const std::exception& e)
	{
		fmt::print("Error: {}", e.what());
	}

	if (argc < 2)
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cin.get();
	}
}