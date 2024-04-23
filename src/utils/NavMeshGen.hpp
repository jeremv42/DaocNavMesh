#pragma once

#include "../common.hpp"
#include "Mesh.hpp"
#include "../../recast/Include/Recast.h"

struct NavMeshGen
{
	rcConfig config;
	rcContext context;

	NavMeshGen(rcConfig const &config) : config(config) {}

	void operator()(Mesh const &mesh, glm::mat4 const &world)
	{
		if (mesh.indices.empty())
			return;
		
	}
};
