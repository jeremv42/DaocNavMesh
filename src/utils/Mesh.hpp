#pragma once

#include "../common.hpp"
#include "./AABB.hpp"

struct Mesh
{
	std::string name;
	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;
	AABB boundingBox;

	void update_boundings() {
		boundingBox = AABB(vertices);
	}

	static Mesh create_cylinder(float radius, float heightRatio = 10);
};
