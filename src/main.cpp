#include "daoc/fs/FileSystem.hpp"
#include "daoc/world/Region.hpp"
#include <Recast.h>

int main(int ac, char const *const *av)
{
	DEBUG_PRINT("split: 'test/ABC\\123\\CBA/a'\n");
	auto parts = string_split("test/ABC\\123\\CBA/a", std::vector<std::string> {"/", "\\"});
	for (auto part: parts)
		DEBUG_PRINT("part: '%s'\n", part.c_str());

	DAOC::Mpk::load(av[1]);

	auto fs = DAOC::FileSystem("E:\\Games\\Daoc_off");
	auto regions = DAOC::Region::LoadAll(fs);
	regions[1]->zones[0].load(fs);

	return 0;
}
