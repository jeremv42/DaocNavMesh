#pragma once

#include <iostream>
#include <ranges>
#include "../../common.hpp"

namespace DAOC
{
	class Mpk
	{
	public:
		struct Entry
		{
			std::vector<char> compressed;
			std::vector<char> uncompressed;
			uint32_t data_size;
		};

	private:
		std::map<std::string, Entry> _files;
		mutable std::mutex _mutex;

	public:
		std::map<std::string, Entry> const &files() const { return this->_files; }
		std::vector<char> const* get_file(std::string file_name);

		static std::unique_ptr<Mpk> load(std::filesystem::path const &path);
		static std::unique_ptr<Mpk> load(std::istream &&in);
	};
}
