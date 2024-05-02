#pragma once

#include "../common.hpp"
#include "Mesh.hpp"

struct WavefrontObjWriter
{
	std::ostream &out;
	size_t vertice_count = 1;

	WavefrontObjWriter(std::ostream &out) : out(out) {}

	void operator()(Mesh const &mesh, glm::mat4 const &world)
	{
		if (mesh.indices.empty())
			return;
		out << "o " << mesh.name << "\n";
		for (auto vertex : mesh.vertices)
		{
			auto v = world * glm::vec4(vertex, 1);
			out << std::format("v {} {} {}\n", v.x, v.z, v.y);
		}
		for (size_t i = 2; i < mesh.indices.size(); i += 3)
			out << std::format("f {} {} {}\n", (mesh.indices[i - 2] + vertice_count), (mesh.indices[i - 1] + vertice_count), (mesh.indices[i] + vertice_count));

		vertice_count += mesh.vertices.size();
	}
};
