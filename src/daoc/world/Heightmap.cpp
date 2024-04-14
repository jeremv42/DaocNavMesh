#include "Heightmap.hpp"
#include "../fs/PCXImage.hpp"
#include "Zone.hpp"

using namespace DAOC;

Heightmap::Heightmap(Zone const &zone, PCXImage const &terrain, int scale_factor, PCXImage const &offset, int offset_factor)
	: width(zone.width), height(zone.height)
{
	heightmap.resize(terrain.data.size());
	for (size_t i = 0; i < heightmap.size(); ++i)
	{
		heightmap[i] = terrain.data[i] * scale_factor + offset.data[i] * offset_factor;
	}
}
