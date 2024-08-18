#include "River.hpp"
#include "../../libs/inipp.hpp"

using namespace DAOC;

River::River(inipp::Ini<char> const &sector, std::string const &section)
{
	int height = sector.get_value(section, "height", 0);
	int point_count = sector.get_value(section, "bankpoints", 0);
	this->points.resize(point_count * 2);
	for (int j = 0; j < point_count; ++j)
	{
		auto left = sector.get_value(section, std::format("left{:02}", j), "0,0,0");
		auto arr = string_split(left, {","});
		this->points[j * 2 + 0] = glm::vec3(std::stof(arr[0]) * 256, std::stof(arr[1]) * 256, height);

		auto right = sector.get_value(section, std::format("right{:02}", j), "0,0,0");
		arr = string_split(right, {","});
		this->points[j * 2 + 1] = glm::vec3(std::stof(arr[0]) * 256, std::stof(arr[1]) * 256, height);
	}
}

Mesh River::get_mesh() const
{
	Mesh m;
	m.name = "river";
	m.vertices.resize(points.size());
	for (size_t i = 0; i < points.size(); ++i)
		m.vertices[i] = glm::vec3(points[i].x, points[i].y, points[i].z);
	m.indices.reserve((m.vertices.size() - 2) * 3);
	for (uint16_t idx = 2; idx < m.vertices.size(); ++idx)
	{
		m.indices.push_back(idx - 2);
		m.indices.push_back(idx - 1);
		m.indices.push_back(idx - 0);
	}
	m.update_boundings();
	return m;
}
