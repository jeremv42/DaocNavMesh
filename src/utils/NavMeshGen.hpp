#pragma once

#include "../common.hpp"
#include "Mesh.hpp"

struct NavMeshGen
{
	struct _Implem;
	_Implem* _implem;

	NavMeshGen();
	~NavMeshGen();
	void operator()(Mesh const &mesh, glm::mat4 const &world);
	void save(std::string const &filename);
};
