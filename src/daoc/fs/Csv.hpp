#pragma once

#include "../../common.hpp"

namespace DAOC
{
	// doesn't manage quoted values
	std::vector<std::vector<std::string>> parse_csv(std::istream &in, std::string const &delimeter = ",")
	{
		std::vector<std::vector<std::string>> rows;
		std::string line;
		while (std::getline(in, line))
			rows.emplace_back(string_split(line, {delimeter}));
		return rows;
	}
}
