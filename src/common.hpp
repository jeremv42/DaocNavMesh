#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vector>

#ifdef _DEBUG
#include <cstdio>
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

inline std::pair<size_t, std::string> string_find_any(std::string str, std::vector<std::string> const &patterns)
{
	size_t pos = 0;
	while (str.size())
	{
		for (auto const &pattern : patterns)
		{
			if (str.starts_with(pattern))
				return std::make_pair(pos, pattern);
		}
		str.erase(0, 1);
		pos += 1;
	}
	return std::make_pair(std::string::npos, "");
}

inline std::vector<std::string> string_split(std::string str, std::vector<std::string> const &delimeters)
{
	std::vector<std::string> parts;
	size_t pos = 0;
	auto it = string_find_any(str, delimeters);
	while (it.first != std::string::npos)
	{
		parts.push_back(str.substr(0, it.first));
		str.erase(0, it.first + it.second.size());
		it = string_find_any(str, delimeters);
	}
	parts.push_back(str);
	return parts;
}

namespace DAOC
{
	class FileSystem;
}
