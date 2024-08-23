#pragma once
#include "../../common.hpp"
#include "Zone.hpp"

namespace DAOC
{
	struct Region
	{
		uint16_t id;
		std::string sub_path = "";
		std::vector<Zone> zones;
		std::vector<Mesh> zone_joins;

		void load(Game &game)
		{
			for (auto &z : zones)
				if (z.id == 168)
					z.load(game);
			for (size_t i = 0; i < zones.size(); ++i)
				for (size_t j = i + 1; j < zones.size(); ++j)
					_create_zone_join(game, zones[i], zones[j]);
		}
		void visit(Game &game, std::function<void(Mesh const &mesh, glm::mat4 const &world)> const &visitor)
		{
			for (auto &z : zones)
				if (z.id == 168)
					z.visit(game, visitor);
			// for (auto const &mesh : zone_joins)
			// 	visitor(mesh, glm::mat4(1));
		}

		void _create_zone_join(Game &game, Zone &z1, Zone &z2);

		static std::map<int, std::unique_ptr<Region>> load_regions(Game &game);
	};
}
