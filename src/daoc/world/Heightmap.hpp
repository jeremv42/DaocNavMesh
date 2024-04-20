#pragma once
#include "../../common.hpp"

namespace DAOC
{
	struct PCXImage;

	struct Heightmap
	{
		int width;
		int height;
		std::vector<int> heightmap;

		Heightmap(PCXImage const &terrain, int scale_factor, PCXImage const &offset, int offset_factor);

		void write_obj(std::ostream &out, size_t &vertex_count) const;
	};
}
