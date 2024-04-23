#include "Region.hpp"
#include "../../libs/inipp.hpp"

using namespace DAOC;

std::map<int, std::unique_ptr<Region>> Region::load_regions(FileSystem &daoc_fs)
{
	auto zonesdat = daoc_fs.open("zones/zones.mpk/zones.dat");
	inipp::Ini<char> ini;
	ini.parse(*zonesdat);
	for (auto const &err : ini.errors)
		DEBUG_PRINT("Error %s\n", err.c_str());

	std::vector<Zone> zones;
	std::map<int, std::unique_ptr<Region>> regions;
	for (auto sect : ini.sections)
	{
		if (!sect.first.starts_with("region"))
			continue;

		if (sect.second.contains("proxy_region"))
		{
			DEBUG_PRINT("Skip %s, is proxy of %s\n", sect.first.c_str(), sect.second["proxy_region"].c_str());
			continue;
		}

		try
		{
			auto reg = std::make_unique<Region>();
			reg->id = std::stoi(sect.first.substr(6));
			if (sect.second.contains("phousing") && sect.second["phousing"] == "1")
				reg->subPath = "phousing";
			if (sect.second.contains("frontiers") && sect.second["frontiers"] == "1")
				reg->subPath = "frontiers";
			if (sect.second.contains("tutorial") && sect.second["tutorial"] == "1")
				reg->subPath = "tutorial";
			regions[reg->id] = std::move(reg);
		}
		catch (std::exception const &ex)
		{
			DEBUG_PRINT("Can't load ini section %s: %s\n", sect.first.c_str(), ex.what());
		}
	}

	for (auto sect : ini.sections)
	{
		if (!sect.first.starts_with("zone"))
			continue;

		if (sect.second.contains("proxy_zone"))
		{
			DEBUG_PRINT("Skip %s, is proxy of %s\n", sect.first.c_str(), sect.second["proxy_zone"].c_str());
			continue;
		}

		try
		{
			int regionId = std::stoi(sect.second["region"]);
			Zone zone{.region = *regions[regionId]};
			zone.id = std::stoi(sect.first.substr(4));
			zone.name = sect.second["name"];
			zone.type = (ZoneType)std::stoi(sect.second["type"]);
			zone.offset_x = std::stoi(sect.second["region_offset_x"]);
			zone.offset_y = std::stoi(sect.second["region_offset_y"]);
			zone.width = std::stoi(sect.second["width"]);
			zone.height = std::stoi(sect.second["height"]);
			regions[regionId]->zones.emplace_back(std::move(zone));
		}
		catch (std::exception const &ex)
		{
			DEBUG_PRINT("Can't load ini section %s: %s\n", sect.first.c_str(), ex.what());
		}
	}
	return regions;
}
