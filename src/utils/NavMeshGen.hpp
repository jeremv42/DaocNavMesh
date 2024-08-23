#pragma once

#include "../common.hpp"
#include "../daoc/world/Region.hpp"
#include "Mesh.hpp"

struct NavMeshGen
{
	struct _Implem;

	DAOC::Region const &_region;
	_Implem* _implem;

	NavMeshGen(DAOC::Region const &region);
	~NavMeshGen();
	void operator()(Mesh const &mesh, glm::mat4 const &world);
	void save(std::string const &filename);
};
