#ifndef PIXMAP_H_
#define PIXMAP_H_

#include <cstdint>
#include <memory>
#include <vector>

#include "pixmapFile.h"
#include "pixmapInfo.h"
#include "pixmapColorTable.h"

namespace imagelib 
{
	class Pixmap 
	{
	public:
		explicit Pixmap() =  default;
		explicit Pixmap(std::int32_t width, std::int32_t height, bool hasAlpha = false);
		explicit Pixmap(const char* filename);
		~Pixmap() = default;

		bool read(const char* filename);
		void write(const char* filename);
		void fill_region(
			uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
			uint8_t r, uint8_t g, uint8_t b, uint8_t a
			);

	private:
		uint32_t m_rowStride{ 0 };

		std::unique_ptr<PixmapFile> m_fileHeader{ std::make_unique<PixmapFile>() };
		std::unique_ptr<PixmapInfo> m_infoHeader{ std::make_unique<PixmapInfo>() };
		std::unique_ptr<PixmapColorTable> m_colorTableHeader{ std::make_unique<PixmapColorTable>() };

		std::unique_ptr<std::vector<std::uint8_t>> m_data{ new std::vector<uint8_t>{} };

		uint32_t make_stride_aligned(uint32_t stride);
		void check_color_header(const std::unique_ptr<PixmapColorTable>& colorHeaderPtr) const;
		void write_header_and_data(std::ofstream& of) const;
		void write_header(std::ofstream& of) const;

	};

}
#endif // !PIXMAP_H_
