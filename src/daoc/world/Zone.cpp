#include "../../libs/inipp.hpp"
#include "Region.hpp"
#include "Zone.hpp"
#include "../fs/FileSystem.hpp"
#include "../fs/PCXImage.hpp"
#include <fstream>

using namespace DAOC;

std::unique_ptr<std::istream> Zone::openFromDat(DAOC::FileSystem &fs, std::string const &filename)
{
	auto file = std::format("zones/zone{0:03}/dat{0:03}.mpk/{1}", this->id, filename);
	return fs.open(filename, {"newtown", this->region.subPath, ""}, std::vector<std::string>{file});
}

void Zone::load(DAOC::FileSystem &fs)
{
	DEBUG_PRINT("Load zone %s (id:%d, %d,%d  %dx%d)\n", name.c_str(), id, offset_x, offset_y, width, height);
	switch (this->type)
	{
	case ZoneType::Normal:
	{
		auto sector_stream = openFromDat(fs, "sector.dat");
		if (!sector_stream)
			return;
		inipp::Ini<char> sector;
		sector.parse(*sector_stream);

		int scale_factor = sector.get_value("terrain", "scalefactor", 0);
		int offset_factor = sector.get_value("terrain", "offsetfactor", 0);
		auto terrain = PCXImage(*openFromDat(fs, "terrain.pcx"));
		auto offset = PCXImage(*openFromDat(fs, "offset.pcx"));
		auto img = std::ofstream("test.pgm", std::ios::out | std::ios::binary);
		terrain.write(img);
		img.close();
		break;
	}

	default:
	{
		DEBUG_PRINT("ERROR Type %d not implemented\n", type);
		break;
	}
	}
}
