#pragma once

#include "../common.hpp"
#include "fs/FileSystem.hpp"

namespace DAOC
{
	struct Game
	{
		FileSystem &fs;
		std::vector<Mesh> mesh_unique_pool;
		std::map<int, Mesh> mesh_pool;

		Game(FileSystem &fs) : fs(fs) {}
	};
}
