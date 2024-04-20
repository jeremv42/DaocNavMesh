#pragma once

#include "../../common.hpp"

namespace DAOC
{
	struct River
	{
		std::vector<glm::vec3> points;

		River(inipp::Ini<char> const &sector, std::string const &section);

		void write_obj(std::ostream &out, size_t &vertex_count) const;
	};
}
