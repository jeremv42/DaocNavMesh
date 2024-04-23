#pragma once

#include "../../common.hpp"
#include "../../utils/Mesh.hpp"

namespace DAOC
{
	struct River
	{
		std::vector<glm::vec3> points;

		River(inipp::Ini<char> const &sector, std::string const &section);

		Mesh get_mesh() const;
	};
}
