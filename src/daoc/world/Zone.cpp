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
			float scale = std::stof(row[7]);
			if (std::fabs(scale) > 0.00001f)
				scale = scale / 100;
			else
				scale = 1;
			glm::quat rotation(glm::vec3(0, 0, std::stof(row[6]) / 360 * std::numbers::pi_v<float>));
			if (row.size() > 16)
				rotation = glm::angleAxis(std::stof(row[15]), glm::vec3(std::stof(row[16]), std::stof(row[17]), std::stof(row[18])));

			glm::mat4x4 world = glm::mat4x4(1);
			world = glm::translate(world, translation);
			world *= glm::mat4_cast(rotation);
			world = glm::scale(world, glm::vec3(scale, scale, scale));
			fixtures.push_back(Fixture{
				.id = std::stoi(row[0]),
				.nif_id = std::stoi(row[1]),
				.world = world,
				.collide = row[8] != "0",
				.collide_radius = std::stoi(row[9]),
				.unique_id = std::stoi(row[14]),
			});
		}

		auto mapobj = std::ofstream(std::format("zone_{:03}.obj", this->id).c_str());
		size_t vertex_count = 0;
		mapobj << "o heightmap" << std::endl;
		heightmap->write_obj(mapobj, vertex_count);
		for (auto i = 0; i < rivers.size(); ++i)
		{
			mapobj << "o river" << i << std::endl;
			rivers[i].write_obj(mapobj, vertex_count);
		}
		for (auto &fixture : fixtures)
			fixture.write_obj(*this, fs, mapobj, vertex_count);
		mapobj.close();
		break;
	}

	default:
	{
		DEBUG_PRINT("ERROR Type %d not implemented\n", type);
		break;
	}
	}
}

void Fixture::write_obj(Zone &zone, FileSystem &fs, std::ostream &out, size_t &vertex_count)
{
	Niflib::NiObject *nif = nullptr;
	if (!zone.nifs_loaded.contains(this->nif_id))
	{
		auto nif_it = zone.nifs.find(this->nif_id);
		if (nif_it == zone.nifs.end())
		{
			DEBUG_PRINT("Fixture %d: unknown nif id %d in zone %d\n", this->id, this->nif_id, zone.id);
			return;
		}
		auto nif_stream = zone.find_nif(fs, nif_it->second);
		if (nif_stream == nullptr)
		{
			DEBUG_PRINT("Fixture %d: can't load nif id %d (%s) in zone %d\n", this->id, this->nif_id, nif_it->second.c_str(), zone.id);
			return;
		}

		zone.nifs_loaded[this->nif_id] = Niflib::ReadNifTree(*nif_stream);
	}
	nif = (Niflib::NiObject *)zone.nifs_loaded[this->nif_id];
	DEBUG_PRINT("%s\n", nif->asString(true).c_str());

	std::vector<glm::vec3> vertices;
	std::vector<uint16_t> indices;
	if (Niflib::TryExtractMesh(nif, this->world, vertices, indices))
	{
		auto nif_it = zone.nifs.find(this->nif_id);
		out << "o nif_" << this->id << "__" << nif_it->second << std::endl;
		for (auto const &v : vertices)
			out << "v " << v.x << " " << v.z << " " << v.y << std::endl;
		for (size_t idx = 0; idx < indices.size(); idx += 3)
			out << "f " << vertex_count + indices[idx] + 1 << " " << vertex_count + indices[idx + 1] + 1 << " " << vertex_count + indices[idx + 2] + 1 << std::endl;
		out << std::endl;
		vertex_count += vertices.size();
	}
}
