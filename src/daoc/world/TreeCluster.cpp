#include "TreeCluster.hpp"
#include "../fs/Csv.hpp"
#include "../fs/FileSystem.hpp"

using namespace DAOC;

static std::map<std::string, std::vector<Tree>> _clusters;

void DAOC::treecluster_init(FileSystem &fs)
{
	auto file_csv = fs.open("zones/trees/tree_clusters.mpk/tree_clusters.csv");
	if (!file_csv)
		throw std::runtime_error("can't load zones/trees/tree_clusters.mpk/tree_clusters.csv");
	auto rows = parse_csv(*file_csv);
	for (auto it = ++rows.begin(); it != rows.end(); ++it)
	{
		auto &row = *it;
		if (row.size() < 5)
			continue;
		std::vector<Tree> trees;
		auto key = row[0];
		std::transform(key.begin(), key.end(), key.begin(), [](char c) { return std::tolower(c); });
		auto nif = row[1];
		for (auto i = 2; i < row.size(); i += 3)
		{
			try
			{
				auto pos = glm::vec3(
					-std::stof(row[i + 0]),
					-std::stof(row[i + 1]),
					std::stof(row[i + 2]));
				if (pos.x != 0 || pos.y != 0 || pos.z != 0)
					trees.push_back(Tree{
						.nif = nif,
						.position = pos,
					});
			}
			catch (...)
			{
				break;
			}
		}
		if (!trees.empty())
			_clusters[key] = std::move(trees);
	}
}
std::vector<Tree> DAOC::treecluster_find(std::string nif_name)
{
	std::transform(nif_name.begin(), nif_name.end(), nif_name.begin(), [](char c) { return std::tolower(c); });
	auto it = _clusters.find(nif_name);
	if (it == _clusters.end())
		return {};
	return it->second;
}
