#pragma once

#include "../../common.hpp"

namespace DAOC
{
	struct Tree
	{
		std::string nif;
		glm::vec3 position;
	};

	void treecluster_init(FileSystem &fs);
	std::vector<Tree> treecluster_find(std::string nif_name);
}
