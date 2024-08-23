#pragma once

#include "../common.hpp"

struct Mesh
{
	std::string name;
	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;
	AABB bounds;

	void update_boundings() {
		bounds = AABB(vertices);
	}

	static Mesh create_cylinder(float radius, float heightRatio = 10);
};
