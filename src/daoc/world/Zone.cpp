#include <fstream>

#include "../../libs/inipp.hpp"
#include "../../libs/niflib.hpp"
#include "../fs/Csv.hpp"
#include "../fs/FileSystem.hpp"
#include "../fs/PCXImage.hpp"
#include "Region.hpp"
#include "Zone.hpp"

using namespace DAOC;

std::unique_ptr<std::istream> Zone::open_from_dat(DAOC::FileSystem &fs, std::string const &filename)
{
	auto file = std::format("zones/zone{0:03}/dat{0:03}.mpk", this->id);
	return fs.open(filename, {"newtown", this->region.subPath, ""}, std::vector<std::string>{file});
}

std::unique_ptr<std::istream> Zone::find_file(DAOC::FileSystem &fs, std::string const &filename)
{
	auto fromCsv = std::format("zones/zone{0:03}/csv{0:03}.mpk", this->id);
	auto fromDat = std::format("zones/zone{0:03}/dat{0:03}.mpk", this->id);
	return fs.open(filename, {"newtown", this->region.subPath, ""}, std::vector<std::string>{fromCsv, fromDat});
}

std::unique_ptr<std::istream> Zone::find_nif(DAOC::FileSystem &fs, std::string filename)
{
	std::transform(filename.begin(), filename.end(), filename.begin(), [](char c)
				   { return std::tolower(c); });
	auto idx = filename.find(".nif");
	if (idx != std::string::npos)
	{
		auto npk = filename;
		npk.replace(idx, 4, ".npk");
		return fs.open(filename, {"newtown", this->region.subPath, ""}, std::vector<std::string>{"zones/nifs", "nifs"}, std::vector<std::string>{"", npk});
	}
	return fs.open(filename, {"newtown", this->region.subPath, ""}, std::vector<std::string>{"zones/nifs", "nifs"});
}

void Zone::load(DAOC::FileSystem &fs)
{
	DEBUG_PRINT("Load zone %s (id:%d, %d,%d  %dx%d)\n", name.c_str(), id, offset_x, offset_y, width, height);
	switch (this->type)
	{
	case ZoneType::Normal:
	{
		auto sector_stream = open_from_dat(fs, "sector.dat");
		if (!sector_stream)
			return;
		inipp::Ini<char> sector;
		sector.parse(*sector_stream);

		auto terrain = PCXImage(*open_from_dat(fs, "terrain.pcx"));
		auto offset = PCXImage(*open_from_dat(fs, "offset.pcx"));
		heightmap = std::make_unique<Heightmap>(terrain, sector.get_value("terrain", "scalefactor", 0), offset, sector.get_value("terrain", "offsetfactor", 0));

		int river_count = sector.get_value("waterdefs", "num", 0);
		for (int i = 0; i < river_count; ++i)
		{
			auto section = std::format("river{:02}", i);
			if (!sector.sections.contains(section))
				continue;
			this->rivers.emplace_back(sector, section);
		}

		auto nifs_csv = parse_csv(*find_file(fs, "nifs.csv"));
		for (int i = 2; i < nifs_csv.size(); ++i)
			nifs[std::stoi(nifs_csv[i][0])] = nifs_csv[i][2];
		auto fixtures_csv = parse_csv(*find_file(fs, "fixtures.csv"));
		for (int i = 2; i < fixtures_csv.size(); ++i)
		{
			auto const &row = fixtures_csv[i];
			if (row.size() < 5)
				continue;
			glm::vec3 translation(std::stof(row[3]), std::stof(row[4]), std::stof(row[5]));
			if (row[11] == "1")
				translation.z = this->get_ground_height(glm::vec2(translation.x, translation.y));
			float scale = std::stof(row[7]);
			if (std::fabs(scale) > 0.00001f)
				scale = scale / 100;
			else
				scale = 1;
			glm::quat rotation(glm::vec3(0, 0, std::stof(row[6]) / 180.f * std::numbers::pi_v<float>));
			if (row.size() > 16)
				rotation = glm::angleAxis(std::stof(row[15]), glm::vec3(std::stof(row[16]), std::stof(row[17]), -std::stof(row[18])));

			glm::mat4x4 world = glm::mat4x4(1);
			world = glm::translate(world, translation);
			world *= glm::mat4_cast(rotation);
			world = glm::scale(world, glm::vec3(scale, -scale, scale));
			fixtures.push_back(Fixture{
				.zone = this,
				.id = std::stoi(row[0]),
				.nif_id = std::stoi(row[1]),
				.world = world,
				.collide = row[8] != "0",
				.collide_radius = std::stoi(row[9]),
				.unique_id = std::stoi(row[14]),
			});
		}
		break;
	}

	default:
	{
		DEBUG_PRINT("ERROR Type %d not implemented\n", type);
		break;
	}
	}
}

std::vector<Mesh> const &Fixture::get_meshes(FileSystem &fs)
{
	if (zone->nifs_loaded.contains(this->nif_id))
		return zone->nifs_loaded[this->nif_id];

	auto nif_it = zone->nifs.find(this->nif_id);
	if (nif_it == zone->nifs.end())
	{
		DEBUG_PRINT("Fixture %d: unknown nif id %d in zone %d\n", this->id, this->nif_id, zone->id);
		return zone->nifs_loaded[this->nif_id];
	}
	auto nif_stream = zone->find_nif(fs, nif_it->second);
	if (nif_stream == nullptr)
	{
		DEBUG_PRINT("Fixture %d: can't load nif id %d (%s) in zone %d\n", this->id, this->nif_id, nif_it->second.c_str(), zone->id);
		return zone->nifs_loaded[this->nif_id];
	}

	auto nif = Niflib::ReadNifTree(*nif_stream);
	Mesh m;
	m.name = nif_it->second;
	if (Niflib::TryExtractMesh(nif, glm::mat4(1), m.vertices, m.indices))
		zone->nifs_loaded[this->nif_id].push_back(m);
	return zone->nifs_loaded[this->nif_id];
}

void Zone::visit(FileSystem &fs, std::function<void(Mesh const &mesh, glm::mat4 const &world)> const &visitor)
{
	auto world = glm::mat4(1);
	world = glm::translate(world, glm::vec3(this->offset_x * 8192, this->offset_y * 8192, 0));
	if (this->heightmap)
	{
		auto m = this->heightmap->get_mesh();
		m.name = std::format("z{:03}_{}", this->id, m.name);
		visitor(m, world);
	}

	auto river_idx = 1;
	for (auto &river : this->rivers)
	{
		auto m = river.get_mesh();
		m.name = std::format("z{:03}_{}{:02}", this->id, m.name, river_idx++);
		visitor(m, world);
	}

	for (auto &fix : this->fixtures)
	{
		auto meshes = fix.get_meshes(fs);
		for (auto &m : meshes)
		{
			m.name = std::format("z{:03}_nif{}_{}", this->id, fix.unique_id, m.name);
			visitor(m, world * fix.world);
		}
	}
}
