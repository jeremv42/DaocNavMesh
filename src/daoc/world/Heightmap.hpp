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

		float get_height(glm::vec2 pos) const;
		Mesh get_mesh() const;
	};
}
