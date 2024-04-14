#pragma once
#include "../../common.hpp"
#include "../fs/FileSystem.hpp"
#include "Zone.hpp"

namespace DAOC
{
	struct Region
	{
		uint16_t id;
		std::string subPath = "";
		std::vector<Zone> zones;

		static std::map<int, std::unique_ptr<Region>> LoadAll(FileSystem &daoc_fs);
	};
}
