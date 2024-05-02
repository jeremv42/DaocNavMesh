#include <format>
#include <fstream>
#include "FileSystem.hpp"
#include "../world/Zone.hpp"

using namespace DAOC;

struct membuf : std::streambuf
{
	membuf(char const *base, size_t size)
	{
		char *p(const_cast<char *>(base));
		this->setg(p, p, p + size);
	}
};
struct imemstream : virtual membuf, std::istream
{
	imemstream(char const *base, size_t size)
		: membuf(base, size), std::istream(static_cast<std::streambuf *>(this))
	{
	}
	virtual ~imemstream() {}
};

std::unique_ptr<std::istream> FileSystem::open(std::string file)
{
	std::transform(file.begin(), file.end(), file.begin(), [](char c) { return std::tolower(c); });

	std::filesystem::path systemPath = this->_root;
	auto file_parts = string_split(file, {"/", "\\"});
	std::string mpkPath = "";

	for (auto it = file_parts.begin(); it != file_parts.end(); ++it)
	{
		systemPath /= *it;
		if (it->ends_with(".npk") || it->ends_with(".mpk"))
		{
			while (++it != file_parts.end())
				mpkPath += *it;
			break;
		}
	}

	if (!std::filesystem::exists(systemPath) || std::filesystem::is_directory(systemPath))
		return nullptr;

	PRINT_DEBUG("Open system='{}' mpk='{}'\n", systemPath.string(), mpkPath);
	if (mpkPath.size())
	{
		Mpk* mpk = nullptr;
		{
			std::lock_guard lock(this->_mpk_mutex);
			if (!this->_mpks.contains(systemPath.string()))
				this->_mpks[systemPath.string()] = Mpk::load(systemPath);
			mpk = this->_mpks[systemPath.string()].get();
		}
		auto file = mpk->get_file(mpkPath);
		return std::unique_ptr<std::istream>(new imemstream(file->data(), file->size()));
	}
	return std::unique_ptr<std::istream>(new std::ifstream(systemPath, std::ios::in | std::ios::binary));
}

std::vector<std::string> FileSystem::make_permutations(std::vector<std::string> const& prefixes, std::vector<std::string> const& suffixes)
{
	std::vector<std::string> res;
	res.reserve(prefixes.size() * suffixes.size());
	for (auto const& pre : prefixes)
		for (auto const& suf : suffixes)
			res.emplace_back(std::format("{}/{}", pre, suf));
	return res;
}
