#include "NavMeshGen.hpp"
#include "../../recast/Include/DetourNavMeshBuilder.h"
#include "../../recast/Include/DetourNavMeshQuery.h"
#include "../../recast/Include/Recast.h"

using namespace DAOC;

struct NavMeshGen::_Implem
{
	rcConfig config;
	rcContext context;

	std::vector<std::tuple<Mesh, glm::mat4>> meshes;
};

NavMeshGen::NavMeshGen()
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

/*
	int width = 512;
	int height = 512;
	auto pair = mesh_big.get_bounds();
	float bmin[3] = {pair.first.x, pair.first.y, pair.first.z};
	float bmax[3] = {pair.second.x, pair.second.y, pair.second.z};
	float cs, ch;
	if (!rcCreateHeightfield(&_implem->context, solid, width, height, bmin, bmax, cs, ch))
		throw std::runtime_error("rcCreateHeightfield failed");
*/
	
}
