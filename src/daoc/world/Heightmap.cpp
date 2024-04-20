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

void Heightmap::write_obj(std::ostream &out, size_t &vertex_count) const
{
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;

	vertices.reserve(height * width);
	indices.reserve(height * width * 6);

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			vertices.emplace_back(x * 256, heightmap[y * width + x], y * 256);
	for (int y = 1; y < height; ++y)
	{
		for (int x = 1; x < width; ++x)
		{
			// first triangle
			indices.push_back((y - 1) * width + x - 1);
			indices.push_back((y - 1) * width + x);
			indices.push_back(y * width + x - 1);
			// second triangle
			indices.push_back((y - 1) * width + x);
			indices.push_back(y * width + x);
			indices.push_back(y * width + x - 1);
		}
	}

	for (auto const &v : vertices)
		out << "v " << v.x << " " << v.y << " " << v.z << std::endl;
	for (int idx = 2; idx < indices.size(); idx += 3)
		out << "f " << vertex_count + indices[idx - 2] + 1 << " " << vertex_count + indices[idx - 1] + 1 << " " << vertex_count + indices[idx] + 1 << std::endl;

	vertex_count += vertices.size();
}
