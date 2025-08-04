#include "NavMeshGen.hpp"
#include "../../recast/Include/DetourNavMeshBuilder.h"
#include "../../recast/Include/DetourNavMeshQuery.h"
#include "../../recast/Include/Recast.h"

#include "WaveObjWriter.hpp"

#include <set>

using namespace DAOC;

struct MeshInstance
{
	Mesh const *mesh;
	glm::mat4 world;
	AABB box;

	MeshInstance(Mesh const *mesh, glm::mat4 const &world)
		: mesh(mesh),
		  world(world)
	{
		this->update_box();
	}

	glm::vec3 position() const { return glm::vec3(this->world[3]); }

	void update_box()
	{
		this->box = mesh->bounds.translate(this->position());
	}
};

struct NavMeshGen::_Implem
{
	rcConfig config;
	rcContext context;

	std::vector<MeshInstance> meshes;
};

NavMeshGen::NavMeshGen(DAOC::Region const &region) : _region(region)
{
	_implem = new _Implem{
		.config = rcConfig(),
		.context = rcContext(),
	};
}
NavMeshGen::~NavMeshGen()
{
	delete this->_implem;
}

void NavMeshGen::operator()(Mesh const &mesh, glm::mat4 const &world)
{
	if (mesh.indices.empty())
		return;
	_implem->meshes.emplace_back(mesh, world);

	static std::set<std::string> names;
	if (names.contains(mesh.name)) return;
	names.insert(mesh.name);
	std::cout << "add " << mesh.name << std::endl;
}

void NavMeshGen::save(std::string const &filename)
{
	int _tile_size = 1024;
	float _cell_size = 4;
	float _cell_height = 20;
	float _agent_radius = 16;
	float _agent_height = 80;
	float _agent_max_climb = 30;
	float _agent_max_slope = 70;
	float _edge_max_length = 128;
	float _edge_max_error = 1.25f;
	float _region_min_size = 64;
	float _region_merge_size = 8;
	int _vertices_max_per_poly = 6;
	float _detail_sample_dist = 1;
	float _detail_sample_max_error = 1;

	dtNavMesh navmesh;
	rcHeightfield solid;
	rcContext ctxt;
	rcConfig cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.cs = _cell_size;
	cfg.ch = _cell_height;
	cfg.walkableSlopeAngle = _agent_max_slope;
	cfg.walkableHeight = (int)ceilf(_agent_height / cfg.ch);
	cfg.walkableClimb = (int)floorf(_agent_max_climb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(_agent_radius / cfg.cs);
	cfg.maxEdgeLen = (int)(_edge_max_length / _cell_size);
	cfg.maxSimplificationError = _edge_max_error;
	cfg.minRegionArea = (int)rcSqr(_region_min_size);
	cfg.mergeRegionArea = (int)rcSqr(_region_merge_size);
	cfg.maxVertsPerPoly = _vertices_max_per_poly;
	cfg.tileSize = _tile_size;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = _detail_sample_dist < 0.9f ? 0 : _cell_size * _detail_sample_dist;
	cfg.detailSampleMaxError = _cell_height * _detail_sample_max_error;

	for (auto &z : this->_region.zones)
	{
		if (z.id != 168)
			continue;
		for (auto y = z.offset_y * 8192; y < (z.offset_y + z.height) * 8192; y += _tile_size)
		{
			for (auto x = z.offset_x * 8192; x < (z.offset_x + z.width) * 8192; x += _tile_size)
			{
				Mesh mesh_merged;
				AABB bounds({glm::vec3(x, y, 0), glm::vec3(x + _tile_size, y + _tile_size, 65000)});
				for (auto &m : this->_implem->meshes)
				{
					if (m.mesh.vertices.size() && m.box.intersects(bounds))
					{
						auto idx = mesh_merged.vertices.size();
						for (auto vertex : m.mesh->vertices)
						{
							auto v = m.world * glm::vec4(vertex, 1);
							mesh_merged.vertices.push_back(v);
						}
						for (auto i : m.mesh->indices)
							mesh_merged.indices.push_back(idx + i);
					}
				}
				mesh_merged.update_boundings();
				bounds.min.z = mesh_merged.bounds.min.z;
				bounds.max.z = mesh_merged.bounds.max.z;

				std::ofstream file(std::format("zone_{}_patch_{}x{}.obj", z.id, x, y));
				WavefrontObjWriter obj(file);
				obj(mesh_merged, glm::identity<glm::mat4>());
			}
		}
	}

	/*
		const float tcs = _tile_size * _cell_size;
		int width = 1024;
		int height = 1024;
		auto bbox = AABB({glm::vec3(0, 0, 0), glm::vec3(width, 65000, height)});
		for (int y = 0; y <)
			// auto pair = mesh_big.get_bounds();
			float bmin[3] = {bbox.min.x, bbox.min.y, bbox.min.z};
		float bmax[3] = {bbox.max.x, bbox.max.y, bbox.max.z};
		float cs, ch;
		if (!rcCreateHeightfield(&_implem->context, solid, width, height, bmin, bmax, cs, ch))
			throw std::runtime_error("rcCreateHeightfield failed");
			*/
}
