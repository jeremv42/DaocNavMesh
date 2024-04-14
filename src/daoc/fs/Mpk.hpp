#pragma once

#include <iostream>
#include <ranges>
#include "../../common.hpp"

namespace DAOC
{
	class Mpk
	{
		std::map<std::string, std::vector<char>> _files;

	public:
		std::map<std::string, std::vector<char>> const &files() const { return this->_files; }
		std::vector<char> const *get_file(std::string file_name) const
		{
			auto it = std::find_if(
				_files.begin(),
				_files.end(),
				[&](auto& file) {
					return std::ranges::equal(file.first, file_name, [](char a, char b) { return std::tolower(a) == std::tolower(b);  });
				}
			);
			if (it == this->_files.end())
				return nullptr;
			return &(it->second);
		}

		static Mpk load(std::filesystem::path const &path);
		static Mpk load(std::istream &&in);
	};
}
