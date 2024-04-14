#include <fstream>

#include "Mpk.hpp"
#include "../../libs/miniz.h"

using namespace DAOC;

uint8_t const MPAK_SIGNATURE[] = {'M', 'P', 'A', 'K'};

void read_bytes(std::istream &in, char *buffer, size_t buffer_size)
{
	in.read(buffer, buffer_size);
	if (in.fail())
		throw std::runtime_error("end of stream (read bytes)");
}
int32_t read_int32le(std::istream &in)
{
	int32_t tmp = 0;
	in.read((char *)&tmp, 4);
	if (in.fail())
		throw std::runtime_error("end of stream (read int32le)");
	return tmp;
}
uint32_t read_uint32le(std::istream &in)
{
	uint32_t tmp = 0;
	in.read((char *)&tmp, 4);
	if (in.fail())
		throw std::runtime_error("end of stream (read uint32le)");
	return tmp;
}

std::vector<char> read_zsome(std::istream &in, size_t max_read, unsigned long uncompressed_size)
{
	std::vector<char> compressed_data(max_read);
	in.read(compressed_data.data(), compressed_data.size());
	if (in.fail())
		throw std::runtime_error("end of stream (read zsome)");
	std::vector<char> uncompressed_data(uncompressed_size);
	mz_uncompress((uint8_t *)uncompressed_data.data(), &uncompressed_size, (uint8_t *)compressed_data.data(), compressed_data.size());
	return uncompressed_data;
}

std::string read_zstring(std::istream &in, size_t max_read)
{
	auto data = read_zsome(in, max_read, max_read * 10);
	return std::string((char const *)data.data());
}

struct _MpkItem
{
	char name[256];
	uint32_t timestamp;
	uint32_t unknown4;
	uint32_t data_offset;
	uint32_t data_size;
	uint32_t compressed_offset;
	uint32_t compressed_size;
	uint32_t compressed_crc;
};

Mpk Mpk::load(std::filesystem::path const &path)
{
	DEBUG_PRINT("Load MPK: %s\n", path.string().c_str());
	return Mpk::load(std::ifstream(path, std::ios::binary));
}
Mpk Mpk::load(std::istream &&in)
{
	char sig[sizeof(MPAK_SIGNATURE)];
	in.read(sig, sizeof(MPAK_SIGNATURE));
	if (in.fail())
		throw std::runtime_error("MPAK signature not found: end of stream");
	if (std::memcmp(sig, MPAK_SIGNATURE, sizeof(MPAK_SIGNATURE)))
		throw std::runtime_error("incorrect MPAK signature");
	in.read(sig, 1);

	Mpk mpk;

	uint32_t checksum = read_uint32le(in);
	uint32_t dirsize = read_uint32le(in);
	uint32_t namesize = read_uint32le(in);
	uint32_t filecount = read_uint32le(in);

	checksum ^= 0x03020100;
	dirsize ^= 0x07060504;
	namesize ^= 0x0B0A0908;
	filecount ^= 0x0F0E0D0C;

	auto mpk_name = read_zstring(in, namesize);
	DEBUG_PRINT("MPK name=%s\n", mpk_name.c_str());

	auto directory = read_zsome(in, dirsize, filecount * sizeof(_MpkItem));
	std::vector<_MpkItem> items(directory.size() / sizeof(_MpkItem));
	std::memcpy(items.data(), directory.data(), directory.size());

	auto pos = in.tellg();
	for (auto const &it : items)
	{
		DEBUG_PRINT("%-30s ts:%d unk:%d off:%d size: %d coffset: %d csize: %d crc: %08x\n", it.name, it.timestamp, it.unknown4, it.data_offset, it.data_size, it.compressed_offset, it.compressed_size, it.compressed_crc);

		in.seekg(pos);
		in.seekg(it.compressed_offset, std::ios::cur);
		auto content = read_zsome(in, it.compressed_size, it.data_size);
		content.resize(it.data_size);
		mpk._files.emplace(it.name, content);
	}

	return mpk;
}
