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

void River::write_obj(std::ostream &out, size_t &vertex_count) const
{
	for (auto const &v : points)
		out << "v " << v.x << " " << v.z << " " << v.y << std::endl;
	for (int idx = 3; idx <= points.size(); ++idx)
		out << "f " << vertex_count + idx - 2 << " " << vertex_count + idx - 1 << " " << vertex_count + idx << std::endl;
	out << std::endl;
	vertex_count += points.size();
}
