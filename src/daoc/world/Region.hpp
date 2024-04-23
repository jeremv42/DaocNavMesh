#pragma once
#include "../../common.hpp"
#include "../fs/FileSystem.hpp"
#include "../../utils/Mesh.hpp"
#include "Zone.hpp"

namespace DAOC
{
	struct Region
	{
		uint16_t id;
		std::string subPath = "";
		std::vector<Zone> zones;

		void load(DAOC::FileSystem &fs)
		{
			std::for_each(zones.begin(), zones.end(), [&](auto &z)
						  { z.load(fs); });
		}
		void visit(FileSystem &fs, std::function<void(Mesh const &mesh, glm::mat4 const &world)> const&visitor)
		{
			std::for_each(zones.begin(), zones.end(), [&](auto &z)
						  { z.visit(fs, visitor); });
		}

		static std::map<int, std::unique_ptr<Region>> load_regions(FileSystem &daoc_fs);
	};
}
