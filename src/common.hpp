#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <format>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <string>
#include <vector>
#include "../glm/ext.hpp"
#include "libs/niflib.hpp"

#include "utils/AABB.hpp"
#include "utils/Mesh.hpp"

#ifndef NDEBUG
#include <cstdio>
#define PRINT_DEBUG(fmt, ...) fprintf(stderr, "%s", std::format(fmt, __VA_ARGS__).c_str())
#else
#define PRINT_DEBUG(fmt, ...)
#endif
#define PRINT_ERROR(fmt, ...) fprintf(stderr, "%s", std::format(fmt, __VA_ARGS__).c_str())

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

inline std::ostream& operator<<(std::ostream& str, glm::vec3 const &v)
{
    str << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return str;
}

// Declarations
namespace inipp
{
	template <typename T>
	class Ini;
}
namespace DAOC
{
	struct Game;
	class FileSystem;
}
