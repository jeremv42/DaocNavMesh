#pragma once

#include "../common.hpp"
#include "fs/FileSystem.hpp"

namespace DAOC
{
	struct Game
	{
		FileSystem &fs;

		Game(FileSystem &fs) : fs(fs) {}

		Game(Game const &) = delete;
		Game &operator=(Game &) = delete;
	};
}
