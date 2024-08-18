#include "Region.hpp"
#include "../../libs/inipp.hpp"
#include "../Game.hpp"

using namespace DAOC;

std::map<int, std::unique_ptr<Region>> Region::load_regions(Game &game)
{
	auto zonesdat = game.fs.open("zones/zones.mpk/zones.dat");
	inipp::Ini<char> ini;
	ini.parse(*zonesdat);
	for (auto const &err : ini.errors)
		PRINT_ERROR("Error {}\n", err);

	std::vector<Zone> zones;
	std::map<int, std::unique_ptr<Region>> regions;
	for (auto sect : ini.sections)
	{
		if (!sect.first.starts_with("region"))
			continue;

		if (sect.second.contains("proxy_region"))
		{
			PRINT_DEBUG("Skip {}, is proxy of {}\n", sect.first, sect.second["proxy_region"]);
			continue;
		}

		try
		{
			auto reg = std::make_unique<Region>();
			reg->id = std::stoi(sect.first.substr(6));
			if (sect.second.contains("phousing") && sect.second["phousing"] == "1")
				reg->sub_path = "phousing";
			if (sect.second.contains("frontiers") && sect.second["frontiers"] == "1")
				reg->sub_path = "frontiers";
			if (sect.second.contains("tutorial") && sect.second["tutorial"] == "1")
				reg->sub_path = "tutorial";
			regions[reg->id] = std::move(reg);
		}
		catch (std::exception const &ex)
		{
			PRINT_ERROR("Can't load ini section {}: {}\n", sect.first, ex.what());
		}
	}

	for (auto sect : ini.sections)
	{
		if (!sect.first.starts_with("zone"))
			continue;

		if (sect.second.contains("proxy_zone"))
		{
			PRINT_DEBUG("Skip {}, is proxy of {}\n", sect.first, sect.second["proxy_zone"]);
			continue;
		}

		try
		{
			int regionId = std::stoi(sect.second["region"]);
			Zone zone{.region = *regions[regionId]};
			zone.id = std::stoi(sect.first.substr(4));
			zone.name = sect.second["name"];
			zone.type = (ZoneType)ini.get_value(sect.first, "type", 0);
			zone.offset_x = std::stoi(sect.second["region_offset_x"]);
			zone.offset_y = std::stoi(sect.second["region_offset_y"]);
			zone.width = std::stoi(sect.second["width"]);
			zone.height = std::stoi(sect.second["height"]);
			regions[regionId]->zones.emplace_back(std::move(zone));
		}
		catch (std::exception const &ex)
		{
			PRINT_ERROR("Can't load ini section {}: {}\n", sect.first, ex.what());
		}
	}
	return regions;
}

void Region::_create_zone_join(Game &game, Zone &z1, Zone &z2)
{
	if (!z1.heightmap || !z2.heightmap)
		return;

	auto rec = [this](Zone &z1, Zone &z2)
	{
		auto z1min = glm::vec<2, int>(z1.offset_x, z1.offset_y);
		auto z1max = z1min + glm::vec<2, int>(z1.width, z1.height);
		auto z2min = glm::vec<2, int>(z2.offset_x, z2.offset_y);
		auto z2max = z2min + glm::vec<2, int>(z2.width, z2.height);

		if (z1.offset_x == z2.offset_x + z2.width)
		{
			auto start = std::max(z1.offset_y, z2.offset_y) * 8192;
			auto end = std::min(z1.offset_y + z1.height, z2.offset_y + z2.height) * 8192;
			if (start < end)
			{
				Mesh mesh;
				mesh.name = std::format("heightmap_join_z{:03}_z{:03}", z1.id, z2.id);
				mesh.vertices.reserve((end - start) * 2);
				for (auto i = start; i < end; i += 256)
				{
					auto h1 = z1.get_height(glm::vec<2, int>(z1.offset_x * 8192, i));
					auto h2 = z2.get_height(glm::vec<2, int>(z1.offset_x * 8192 - 256, i));
					mesh.vertices.push_back(glm::vec3(z1.offset_x * 8192, i, h1));
					mesh.vertices.push_back(glm::vec3(z1.offset_x * 8192 - 256, i, h2));
				}
				mesh.indices.reserve(mesh.vertices.size() * 3);
				for (auto i = 2; i < mesh.vertices.size(); ++i)
				{
					mesh.indices.push_back(i - 2);
					mesh.indices.push_back(i - 1);
					mesh.indices.push_back(i - 0);
				}
				this->zone_joins.emplace_back(std::move(mesh));
			}
		}
		if (z1.offset_y == z2.offset_y + z2.height)
		{
			auto start = std::max(z1.offset_x, z2.offset_x) * 8192;
			auto end = std::min(z1.offset_x + z1.width, z2.offset_x + z2.width) * 8192;
			if (start < end)
			{
				Mesh mesh;
				mesh.name = std::format("heightmap_join_z{:03}_z{:03}", z1.id, z2.id);
				mesh.vertices.reserve((end - start) * 2);
				for (auto i = start; i < end; i += 256)
				{
					auto h1 = z1.get_height(glm::vec<2, int>(i, z1.offset_y * 8192));
					auto h2 = z2.get_height(glm::vec<2, int>(i, z1.offset_y * 8192 - 256));
					mesh.vertices.push_back(glm::vec3(i, z1.offset_y * 8192, h1));
					mesh.vertices.push_back(glm::vec3(i, z1.offset_y * 8192 - 256, h2));
				}
				mesh.indices.reserve(mesh.vertices.size() * 3);
				for (auto i = 2; i < mesh.vertices.size(); ++i)
				{
					mesh.indices.push_back(i - 2);
					mesh.indices.push_back(i - 1);
					mesh.indices.push_back(i - 0);
				}
				this->zone_joins.emplace_back(std::move(mesh));
			}
		}
	};

	rec(z1, z2);
	rec(z2, z1); // will add z1 offset + size == z2 offset
}
