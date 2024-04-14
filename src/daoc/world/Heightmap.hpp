#pragma once
#include "../../common.hpp"

namespace DAOC
{
	struct PCXImage;
	struct Zone;

	struct Heightmap
	{
		int width;
		int height;
		std::vector<int> heightmap;

		Heightmap(Zone const &zone, PCXImage const &terrain, int scale_factor, PCXImage const &offset, int offset_factor);
	};
}
