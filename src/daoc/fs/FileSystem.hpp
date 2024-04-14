#pragma once
#include "../../common.hpp"
#include "Mpk.hpp"

namespace DAOC
{
	struct Zone;

	class FileSystem
	{
		std::filesystem::path _root;
		std::map<std::string, Mpk> _mpks;

	public:
		FileSystem(std::filesystem::path const &root)
			: _root(root)
		{
		}

		std::unique_ptr<std::istream> open(std::string file);
		template <typename... Args>
		std::unique_ptr<std::istream> open(std::string const &file, std::vector<std::string> const &prefixes, Args... rest)
		{
			auto try_paths = make_permutations(prefixes, rest...);
			for (auto const &path : try_paths)
			{
				auto stream = open(path);
				if (stream)
					return stream;
			}
			return nullptr;
		}

	private:
		static std::vector<std::string> make_permutations(std::vector<std::string> const& prefixes, std::vector<std::string> const& suffixes);
		template <typename... Args>
		static std::vector<std::string> make_permutations(std::vector<std::string> const& prefixes, std::vector<std::string> const& suffixes, Args... rest)
		{
			std::vector<std::string> res;
			res.reserve(prefixes.size() * suffixes.size());
			for (auto const& pre : prefixes)
				for (auto const& suf : suffixes)
					if (pre == "")
						res.emplace_back(suf);
					else
						res.emplace_back(std::format("{}/{}", pre, suf));
			return make_permutations(res, rest...);
		}
	};
}
