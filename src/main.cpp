#include "daoc/fs/FileSystem.hpp"
#include "daoc/world/Region.hpp"
#include <Recast.h>

#include <atomic>
#include <thread>

int main(int ac, char const *const *av)
{
	auto fs = DAOC::FileSystem("/Users/jeremy/Library/Containers/com.isaacmarovitz.Whisky/Bottles/BDC6CC95-4C59-4B5A-82D4-B385577AA6F8/drive_c/Program Files (x86)/Electronic Arts/Dark Age of Camelot");
	// auto fs = DAOC::FileSystem("E:\\Games\\Daoc_off");
	auto regions = DAOC::Region::LoadAll(fs);

	std::vector<DAOC::Zone*> todo;
	// all
	if (0)
	{
		for (auto& r : regions)
			for (auto& z : r.second->zones)
				if (z.proxy_zone_id == 0)
					todo.push_back(&z);
	}
	else
	{
		for (auto& z : regions[1]->zones)
			todo.push_back(&z);
	}

	std::atomic_int todo_idx = 0;
	std::vector<std::thread> threads;
	for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i)
	{
		auto& t = threads.emplace_back([&]() {
			DAOC::Zone* z;
			while (true) {
				auto idx = todo_idx++;
				if (idx >= todo.size())
					return;
				z = todo[idx];
				std::cout << std::format("Load zone {}...\n", z->id);
				z->load(fs);
			}
			});
	}

	for (auto& t : threads)
		t.join();

	return 0;
}
