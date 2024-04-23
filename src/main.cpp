#include "daoc/fs/FileSystem.hpp"
#include "daoc/world/Region.hpp"
#include <Recast.h>

#include <atomic>
#include <thread>
#include <fstream>

#include "utils/WaveObjWriter.hpp"

int main(int ac, char const *const *av)
{
	auto fs = DAOC::FileSystem("/Users/jeremy/Library/Containers/com.isaacmarovitz.Whisky/Bottles/BDC6CC95-4C59-4B5A-82D4-B385577AA6F8/drive_c/Program Files (x86)/Electronic Arts/Dark Age of Camelot");
	// auto fs = DAOC::FileSystem("E:\\Games\\Daoc_off");
	auto regions = DAOC::Region::load_regions(fs);

	std::vector<DAOC::Region *> todo_regions;
	std::vector<DAOC::Zone *> todo_zones;
	// all
	if (!0)
	{
		for (auto &r : regions)
			todo_regions.push_back(r.second.get());
	}
	else
	{
		todo_regions.push_back(regions[242].get());
	}

	std::atomic_int todo_idx = 0;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		auto &t = threads.emplace_back([&]()
									   {
			DAOC::Region* r;
			while (true) {
				auto idx = todo_idx++;
				if (idx >= todo_regions.size())
					break;
				r = todo_regions[idx];
				std::cout << std::format("Load region {}...\n", r->id);
				r->load(fs);
				std::ofstream out(std::format("region_{:03}.obj", r->id), std::ios::binary);
				WavefrontObjWriter region(out);
				r->visit(fs, [&](auto &m, auto &w) { region(m, w); });
				out.close();
			}
			DAOC::Zone* z;
			while (true) {
				auto idx = todo_idx++;
				if (idx >= todo_zones.size())
					break;
				z = todo_zones[idx];
				std::cout << std::format("Load zone {}...\n", z->id);
				z->load(fs);
			} });
	}

	for (auto &t : threads)
		t.join();

	return 0;
}
