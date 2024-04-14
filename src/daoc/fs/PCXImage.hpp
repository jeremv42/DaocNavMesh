#pragma once
#include "../../common.hpp"

namespace DAOC
{
	struct PCXImage
	{
		#pragma pack(push, 1)
		struct RGB
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		struct RGBA : public RGB
		{
			uint8_t a;
		};
		struct Header
		{
			uint8_t magic;
			uint8_t version;
			uint8_t encoding;
			uint8_t color_depth;
			uint16_t min_x;
			uint16_t min_y;
			uint16_t max_x;
			uint16_t max_y;
			uint16_t dpi_x;
			uint16_t dpi_y;
			RGB palette[16];
			uint8_t _reserved1;
			uint8_t color_plane_count;
			uint16_t color_plane_row_bytes;
			uint16_t palette_mode;
			uint16_t src_dpi_x;
			uint16_t src_dpi_y;
			uint8_t _reserved2[54];

			int width() const { return max_x - min_x + 1; }
			int height() const { return max_x - min_x + 1; }
			int stride() const { return color_plane_row_bytes * color_plane_count; }
		};
		#pragma pack(pop)

		Header header;
		std::vector<char> data;

		PCXImage(std::istream &in)
		{
			in.read((char *)&header, sizeof(header));
			if (in.fail() || header.magic != 0x0A)
				throw std::runtime_error("invalid PCX image");
			data.resize(header.height() * header.width());
			for (int y = 0; y < header.height(); ++y)
			{
				int x = 0;
				while (x < header.width())
				{
					char b;
					in.read(&b, 1);
					if ((b & 0xC0) != 0xC0)
						data[y * header.width() + x++] = b;
					else
					{
						int count = b & 0x3F;
						char c;
						in.read(&c, 1);
						for (int i = 0; i < count; ++i)
							data[y * header.width() + x++] = c;
					}
				}
			}
		}

		void write(std::ostream& out)
		{
			out << "P5\n" << header.width() << " " << header.height() << "\n";
			out << "255\n";
			for (int y = 0; y < header.height(); ++y)
				out.write(data.data() + y * header.width(), header.width());
		}
	};
}
