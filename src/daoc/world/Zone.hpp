#pragma once
#include "../../common.hpp"

#include "Heightmap.hpp"
#include "River.hpp"

namespace DAOC
{
	struct Region;
	struct Zone;

	enum class ZoneType : int
	{
		Normal = 0,
		City = 1,
		Dungeon = 2,
		SkyCity = 3,
	};

	struct Fixture
	{
		int id;
		int nif_id;
		glm::mat4x4 world;
		bool collide;
		int collide_radius;
		int unique_id;

		void write_obj(Zone &zone, FileSystem &fs, std::ostream &out, size_t &vertex_count);
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
		std::vector<River> rivers;
		std::map<int, std::string> nifs;
		std::map<int, Niflib::NiObjectRef> nifs_loaded;
		std::vector<Fixture> fixtures;

		std::unique_ptr<std::istream> open_from_dat(DAOC::FileSystem &fs, std::string const &filename);
		std::unique_ptr<std::istream> find_file(DAOC::FileSystem &fs, std::string const &filename);
		std::unique_ptr<std::istream> find_nif(DAOC::FileSystem &fs, std::string filename);
		void load(DAOC::FileSystem &fs);
	};
}
