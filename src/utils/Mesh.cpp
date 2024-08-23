#include "Mesh.hpp"

Mesh Mesh::create_cylinder(float radius, float heightRatio)
{
	auto const height = radius * heightRatio;
	Mesh m;
	m.name = "cylinder";
	m.vertices = {
		{radius * 1.00f, radius * 0.00f, height * 0.00f},
		{radius * 1.00f, radius * 0.00f, height * 1.00f},

		{radius * 0.31f, radius * 0.95f, height * 0.00f},
		{radius * 0.31f, radius * 0.95f, height * 1.00f},

		{radius * -0.81f, radius * 0.59f, height * 0.00f},
		{radius * -0.81f, radius * 0.59f, height * 1.00f},

		{radius * -0.81f, radius * -0.59f, height * 0.00f},
		{radius * -0.81f, radius * -0.59f, height * 1.00f},

		{radius * 0.31f, radius * -0.95f, height * 0.00f},
		{radius * 0.31f, radius * -0.95f, height * 1.00f},
	};
	m.indices = {
		0, 2, 1,
		1, 2, 3,
		2, 4, 3,
		3, 4, 5,
		4, 6, 5,
		5, 6, 7,
		6, 8, 7,
		7, 8, 9,
		8, 0, 9,
		9, 0, 1,
	};
	m.update_boundings();
	return m;
}
