#include <fstream>

#include "../../libs/inipp.hpp"
#include "../../libs/niflib.hpp"
#include "../Game.hpp"
#include "../fs/Csv.hpp"
#include "../fs/PCXImage.hpp"
#include "Region.hpp"
#include "TreeCluster.hpp"
#include "Zone.hpp"

using namespace DAOC;

std::unique_ptr<std::istream> Zone::open_from_dat(DAOC::FileSystem &fs, std::string const &filename)
{
	auto file = std::format("zones/zone{0:03}/dat{0:03}.mpk", this->id);
	return fs.open(filename, {"newtown", this->region.sub_path, ""}, std::vector<std::string>{file});
}

std::unique_ptr<std::istream> Zone::find_file(DAOC::FileSystem &fs, std::string const &filename)
{
	auto fromCsv = std::format("zones/zone{0:03}/csv{0:03}.mpk", this->id);
	auto fromDat = std::format("zones/zone{0:03}/dat{0:03}.mpk", this->id);
	return fs.open(filename, {"newtown", this->region.sub_path, ""}, std::vector<std::string>{fromCsv, fromDat});
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
		return fs.open(filename, {"newtown", this->region.sub_path, ""}, std::vector<std::string>{"zones/nifs", "nifs"}, std::vector<std::string>{"", npk});
	}
	return fs.open(filename, {"newtown", this->region.sub_path, ""}, std::vector<std::string>{"zones/nifs", "nifs"});
}

void Zone::load(Game &game)
{
	PRINT_DEBUG("Load zone %s (id:%d, %d,%d  %dx%d)\n", name, id, offset_x, offset_y, width, height);
	switch (this->type)
	{
	case ZoneType::Normal:
		_loadNormal(game);
		break;
	case ZoneType::City:
		_loadCity(game);
		break;
	case ZoneType::Dungeon:
		_loadDungeon(game);
		break;

	default:
	{
		PRINT_DEBUG("ERROR Type {} not implemented\n", (int)type);
		break;
	}
	}
}

void Zone::_loadNormal(Game &game)
{
	auto sector_stream = open_from_dat(game.fs, "sector.dat");
	if (!sector_stream)
		return;
	inipp::Ini<char> sector;
	sector.parse(*sector_stream);

	auto terrain = PCXImage(*open_from_dat(game.fs, "terrain.pcx"));
	auto offset = PCXImage(*open_from_dat(game.fs, "offset.pcx"));
	heightmap = std::make_unique<Heightmap>(terrain, sector.get_value("terrain", "scalefactor", 0), offset, sector.get_value("terrain", "offsetfactor", 0));

	int river_count = sector.get_value("waterdefs", "num", 0);
	for (int i = 0; i < river_count; ++i)
	{
		auto section = std::format("river{:02}", i);
		if (!sector.sections.contains(section))
			continue;
		this->rivers.emplace_back(sector, section);
	}

	auto nifs_csv = parse_csv(*find_file(game.fs, "nifs.csv"));
	for (int i = 2; i < nifs_csv.size(); ++i)
		nifs[std::stoi(nifs_csv[i][0])] = nifs_csv[i][2];
	auto fixtures_csv = parse_csv(*find_file(game.fs, "fixtures.csv"));
	for (int i = 2; i < fixtures_csv.size(); ++i)
	{
		auto const &row = fixtures_csv[i];
		if (row.size() < 5)
			continue;
		glm::vec3 translation(std::stof(row[3]), std::stof(row[4]), std::stof(row[5]));
		if (row[11] == "1")
			translation.z = this->get_ground_height(translation);
		float scale = std::stof(row[7]);
		if (std::fabs(scale) > 0.00001f)
			scale = scale / 100;
		else
			scale = 1;
		auto rotation = glm::rotate(glm::mat4(1), std::stof(row[6]) / 180.f * std::numbers::pi_v<float>, glm::vec3(0, 0, 1));
		if (row.size() > 16)
			rotation = glm::rotate(glm::mat4(1), std::stof(row[15]), glm::vec3(std::stof(row[16]), std::stof(row[17]), -std::stof(row[18])));

		_add_fixture(
			Fixture{
				.zone = this,
				.id = std::stoi(row[0]),
				.nif_id = std::stoi(row[1]),
				.world = glm::mat4(1),
				.collide = row[8] != "0",
				.collide_radius = std::stoi(row[9]),
				.unique_id = std::stoi(row[14]),
			},
			translation,
			rotation,
			scale);
	}
}

void Zone::_add_fixture(Fixture &&fixture, glm::vec3 const &translation, glm::mat4 const &rotation, float scale)
{
	glm::mat4x4 world = glm::mat4x4(1);
	world = glm::translate(world, translation);
	world *= rotation;
	world = glm::scale(world, glm::vec3(scale, -scale, scale));
	fixture.world = world;

	auto nif_it = nifs.find(fixture.nif_id);
	if (nif_it == nifs.end())
	{
		fixtures.emplace_back(std::move(fixture));
		return;
	}

	auto cluster = treecluster_find(nif_it->second);
	if (cluster.empty())
	{
		fixtures.emplace_back(std::move(fixture));
		return;
	}

	int i = 0;
	for (auto &tree : cluster)
	{
		auto posw = rotation * glm::vec4(tree.position, 1) * scale;
		auto pos = translation + glm::vec3(posw.x, posw.y, posw.z);
		pos.z = this->get_ground_height(pos);
		world = glm::scale(glm::translate(glm::mat4x4(1), pos), glm::vec3(1, -1, 1));

		fixtures.emplace_back(Fixture{
			.zone = this,
			.id = fixture.id * 100000 + i,
			.nif_id = fixture.nif_id,
			.world = world,
			.collide = fixture.collide,
			.collide_radius = fixture.collide_radius,
			.unique_id = fixture.unique_id * 100000 + i,
		});
		++i;
	}
}

void Zone::_loadCity(Game &game)
{
}

void Zone::_loadDungeon(Game &game)
{
}

std::vector<Mesh *> const &Fixture::get_meshes(Game &game)
{
	if (zone->id == 167)
		PRINT_ERROR("Fixture {} (z{:03}): add nif id {}\n", this->id, zone->id, this->nif_id);

	if (zone->nifs_loaded.contains(this->nif_id))
		return zone->nifs_loaded[this->nif_id];

	auto nif_it = zone->nifs.find(this->nif_id);
	if (nif_it == zone->nifs.end())
	{
		PRINT_ERROR("Fixture {} (z{:03}): unknown nif id {}\n", this->id, zone->id, this->nif_id);
		return zone->nifs_loaded[this->nif_id];
	}

	if (!collide && collide_radius > 0)
	{
		auto mesh = Mesh::create_cylinder(this->collide_radius * 5);
		mesh.update_boundings();
		mesh.name = nif_it->second;
		game.mesh_pool[this->nif_id] = std::move(mesh);
		zone->nifs_loaded[this->nif_id] = {&(game.mesh_pool[this->nif_id])};
		return zone->nifs_loaded[this->nif_id];
	}

	auto nif_stream = zone->find_nif(game.fs, nif_it->second);
	if (nif_stream == nullptr)
	{
		PRINT_ERROR("Fixture {} (z{:03}): can't load nif id {} ({})\n", this->id, zone->id, this->nif_id, nif_it->second);
		return zone->nifs_loaded[this->nif_id];
	}

	auto nif = Niflib::ReadNifTree(*nif_stream);
	Mesh m;
	if (Niflib::TryExtractMesh(nif, glm::mat4(1), m.vertices, m.indices))
	{
		m.name = nif_it->second;
		m.update_boundings();
		game.mesh_pool[this->nif_id] = std::move(m);
		zone->nifs_loaded[this->nif_id].push_back(&(game.mesh_pool[this->nif_id]));
	}
	if (m.vertices.empty())
		PRINT_ERROR("Fixture {} (z{:03}): no mesh found! (nif id: {}, nif: {})\n", this->id, zone->id, this->nif_id, nif_it->second);
	return zone->nifs_loaded[this->nif_id];
}

void Zone::visit(Game &game, std::function<void(Mesh const &mesh, glm::mat4 const &world)> const &visitor)
{
	auto world = glm::mat4(1);
	world = glm::translate(world, glm::vec3(this->offset_x * 8192, this->offset_y * 8192, 0));
	if (this->heightmap)
	{
		auto meshes = this->heightmap->get_meshes();
		for (auto &m : meshes)
		{
			game.mesh_unique_pool.emplace_back(std::move(m));
			auto &mesh = game.mesh_unique_pool.back();
			mesh.name = std::format("z{:03}_{}", this->id, mesh.name);
			visitor(mesh, world);
		}
	}

	auto river_idx = 1;
	for (auto &river : this->rivers)
	{
		game.mesh_unique_pool.emplace_back(river.get_mesh());
		auto &m = game.mesh_unique_pool.back();
		m.name = std::format("z{:03}_{}{:02}", this->id, m.name, river_idx++);
		visitor(m, world);
	}

	for (auto &fix : this->fixtures)
	{
		auto meshes = fix.get_meshes(game);
		for (auto m : meshes)
			visitor(*m, world * fix.world);
	}
}
