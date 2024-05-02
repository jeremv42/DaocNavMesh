#pragma once

#include "../common.hpp"

struct Mesh
{
	std::string name;
	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;

	static Mesh create_radius(int radius);
};
