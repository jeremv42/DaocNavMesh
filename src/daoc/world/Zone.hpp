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
		Zone *zone;
		int id;
		int nif_id;
		glm::mat4x4 world;
		bool collide;
		int collide_radius;
		int unique_id;

		std::vector<Mesh> const &get_meshes(FileSystem &fs);
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
		Region &region;

		std::unique_ptr<Heightmap> heightmap;
		std::vector<River> rivers;
		std::map<int, std::string> nifs;
		std::map<int, std::vector<Mesh>> nifs_loaded;
		std::vector<Fixture> fixtures;

		std::unique_ptr<std::istream> open_from_dat(DAOC::FileSystem &fs, std::string const &filename);
		std::unique_ptr<std::istream> find_file(DAOC::FileSystem &fs, std::string const &filename);
		std::unique_ptr<std::istream> find_nif(DAOC::FileSystem &fs, std::string filename);
		void load(DAOC::FileSystem &fs);

		float get_ground_height(glm::vec2 pos) const
		{
			if (this->heightmap)
				return this->heightmap->get_height(pos);
			return 0.f;
		}

		void visit(FileSystem &fs, std::function<void(Mesh const &mesh, glm::mat4 const &world)> const &visitor);
	};
}
