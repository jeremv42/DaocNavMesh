#pragma once
#include "../../common.hpp"
#include "../../utils/Mesh.hpp"

namespace DAOC
{
	struct PCXImage;

	struct Heightmap
	{
		int width;
		int height;
		std::vector<int> heightmap;

		Heightmap(PCXImage const &terrain, int scale_factor, PCXImage const &offset, int offset_factor);

		float get_height(glm::vec3 pos) const;
		int get_height(glm::vec<2, int> loc) const
		{
			loc /= 256;
			if (loc.x < 0 || loc.y < 0 || loc.x > 255 || loc.y > 255)
				return std::numeric_limits<int>::min();
			return this->heightmap[loc.y * 256 + loc.x];
		}
		Mesh get_mesh() const;
	};
}
