#pragma once

#include "../../common.hpp"

namespace DAOC
{
	// doesn't manage quoted values
	inline std::vector<std::vector<std::string>> parse_csv(std::istream &in, std::string const &delimeter = ",")
	{
		std::vector<std::vector<std::string>> rows;
		std::string line;
		while (std::getline(in, line))
		{
			line.erase(line.find_last_not_of(" \n\r\t") + 1);
			rows.emplace_back(string_split(line, {delimeter}));
		}
		return rows;
	}
}
