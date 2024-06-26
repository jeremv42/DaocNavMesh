#include "Heightmap.hpp"
#include "../fs/PCXImage.hpp"
#include "Zone.hpp"

using namespace DAOC;

Heightmap::Heightmap(PCXImage const &terrain, int scale_factor, PCXImage const &offset, int offset_factor)
	: width(terrain.header.width()), height(terrain.header.height())
{
	heightmap.resize(terrain.data.size());
	for (size_t i = 0; i < heightmap.size(); ++i)
		heightmap[i] = uint8_t(terrain.data[i]) * scale_factor + uint8_t(offset.data[i]) * offset_factor;
}

float Heightmap::get_height(glm::vec3 pos) const
{
	pos /= 256;
	auto xmin = int(pos.x);
	auto xmax = xmin + 1;
	auto ymin = int(pos.y);
	auto ymax = ymin + 1;
	if (xmin < 0 || ymin < 0 || xmax > 255 || ymax > 255)
		return pos.z;
	auto xratio = pos.x - xmin;
	auto yratio = pos.y - ymin;
	auto heightTR = float(heightmap[ymin * 256 + xmax]);
	auto heightBL = float(heightmap[ymax * 256 + xmin]);
	if (xratio + yratio < 1.0f)
	{
		auto height3 = heightmap[ymin * 256 + xmin];
		return ((height3 * (1.f - xratio)) + (heightTR * xratio)) * (1.f - yratio)
			+ heightBL * yratio;
	}
	auto height3 = float(heightmap[ymax * 256 + xmax]);
	return (height3 * xratio + heightBL * (1.f - xratio)) * yratio + heightTR * (1.f - yratio);
}

Mesh Heightmap::get_mesh() const
{
	Mesh m;
	m.name = "heightmap";

	m.vertices.reserve(height * width);
	m.indices.reserve(height * width * 6);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			m.vertices.emplace_back(x * 256, y * 256, heightmap[y * width + x]);
	for (int y = 1; y < height; ++y)
	{
		for (int x = 1; x < width; ++x)
		{
			// first triangle
			m.indices.push_back((y - 1) * width + x - 1);
			m.indices.push_back((y - 1) * width + x);
			m.indices.push_back(y * width + x - 1);
			// second triangle
			m.indices.push_back((y - 1) * width + x);
			m.indices.push_back(y * width + x);
			m.indices.push_back(y * width + x - 1);
		}
	}
	return m;
}

