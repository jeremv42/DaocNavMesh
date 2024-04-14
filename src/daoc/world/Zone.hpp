#pragma once
#include "../../common.hpp"

#include "Heightmap.hpp"

namespace DAOC
{
	struct Region;

	enum class ZoneType : int
	{
		Normal = 0,
		City = 1,
		Dungeon = 2,
		SkyCity = 3,
	};

	struct Zone
	{
		int id;
		ZoneType type = ZoneType::Normal;
		std::string name;
		int offset_x;
		int offset_y;
		int width;
		int height;

		int proxy_zone_id;
		Region& region;

		std::unique_ptr<Heightmap> heightmap;

		std::unique_ptr<std::istream> openFromDat(DAOC::FileSystem &fs, std::string const &filename);
		void load(DAOC::FileSystem &fs);
	};
}
