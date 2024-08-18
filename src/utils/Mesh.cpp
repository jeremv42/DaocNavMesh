#include "Mesh.hpp"

Mesh Mesh::create_cylinder(float radius, float heightRatio)
{
	Mesh m;
	m.name = "cylinder";
	m.vertices = {
		{1.00f, 0.00f, 0.00f},
		{1.00f, 0.00f, 1.00f},

		{0.31f, 0.95f, 0.00f},
		{0.31f, 0.95f, 1.00f},

		{-0.81f, 0.59f, 0.00f},
		{-0.81f, 0.59f, 1.00f},

		{-0.81f, -0.59f, 0.00f},
		{-0.81f, -0.59f, 1.00f},

		{0.31f, -0.95f, 0.00f},
		{0.31f, -0.95f, 1.00f},
	};
	for (auto &v:m.vertices)
		v *= glm::vec3(radius, radius, radius * heightRatio) * 5.f;
	m.indices = {
		0, 1, 2,
		1, 2, 3,
		2, 3, 4,
		3, 4, 5,
		4, 5, 6,
		5, 6, 7,
		6, 7, 8,
		7, 8, 9,
		8, 9, 0,
		9, 0, 1,
	};
	m.update_boundings();
	return m;
}
