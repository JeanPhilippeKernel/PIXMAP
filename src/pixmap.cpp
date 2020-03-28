#include "pixmap.h"

#include <iostream>
#include <fstream>

namespace imagelib
{
	Pixmap::Pixmap(std::int32_t width, std::int32_t height, bool hasAlpha) 
		: Pixmap{}
	{
		if (width <= 0 || height <= 0) {
			throw std::runtime_error("The image width and height must be positive");
		}

		m_infoHeader->height = height;
		m_infoHeader->width = width;
		if (hasAlpha) {
			m_infoHeader->size = sizeof(PixmapInfo) + sizeof(PixmapColorTable);
			m_fileHeader->file_size = sizeof(PixmapInfo) + sizeof(PixmapColorTable) + sizeof(PixmapFile);
			
			m_infoHeader->bit_count = 32;
			m_infoHeader->compression = 3;
			m_rowStride = width * (m_infoHeader->bit_count / 8);
			m_data->resize(height * m_rowStride);
			m_fileHeader->file_size = m_data->size() + m_fileHeader->offset_data;
		}
		else
		{
			m_infoHeader->size = sizeof(PixmapInfo);
			m_fileHeader->file_size = sizeof(PixmapInfo) + sizeof(PixmapFile);

			m_infoHeader->bit_count = 24;
			m_infoHeader->compression = 0;
			m_rowStride = width * (m_infoHeader->bit_count / 8);
			m_data->resize(height * m_rowStride);

			uint32_t newStride = make_stride_aligned(4);
			m_fileHeader->file_size = m_fileHeader->offset_data + m_data->size() + m_infoHeader->height * (newStride - m_rowStride);
		}
	}

	Pixmap::Pixmap(const char* filename) 
		: Pixmap{}
	{
		read(filename);
	}		  

	bool Pixmap::read(const char* filename) 
	{
		bool operationSucceeded{ false };

		std::ifstream inp{ filename, std::ifstream::binary };

		if (inp) {

			inp.read(reinterpret_cast<char*>(m_fileHeader.get()), sizeof(PixmapFile));
			if (m_fileHeader->file_type != 0x4D42) {
				throw std::runtime_error("Error unrecognized file format.");
			}

			inp.read(reinterpret_cast<char*>(m_infoHeader.get()), sizeof(PixmapInfo));
			if (m_infoHeader->bit_count == 32) {
				auto totalSize = sizeof(PixmapInfo) + sizeof(PixmapColorTable);
				if (m_infoHeader->size >= totalSize) {
					inp.read(reinterpret_cast<char*>(m_colorTableHeader.get()), sizeof(PixmapColorTable));
					check_color_header(m_colorTableHeader);
				}
				else {
					std::cerr << "Warning! The file \"" << filename << "\" does not seem to contain  bit mask information " << std::endl;
					throw std::runtime_error("Error unrecognized file format.");
				}
			}
			
			inp.seekg(m_fileHeader->offset_data, std::ifstream::beg);
			if (m_infoHeader->bit_count == 32) {
				m_infoHeader->size = sizeof(PixmapInfo) + sizeof(PixmapColorTable);
				m_fileHeader->offset_data = sizeof(PixmapFile) + sizeof(PixmapInfo) + sizeof(PixmapColorTable);
			}
			else {
				m_infoHeader->size = sizeof(PixmapInfo);
				m_fileHeader->offset_data = sizeof(PixmapFile) + sizeof(PixmapInfo);
			}

			m_fileHeader->file_size = m_fileHeader->offset_data;

			if (m_infoHeader->height < 0) {
				throw std::runtime_error("The program can treat only BMP images with the origin in the bottom left corner !");
			}

			uint32_t byteCount = m_infoHeader->bit_count / 8;
			m_rowStride = m_infoHeader->width * byteCount;
			m_data->resize(m_rowStride *  m_infoHeader->height);
			
			if (m_infoHeader->width % 4 == 0) {
				inp.read(reinterpret_cast<char*>(m_data->data()), m_data->size());
			}
			else {
				uint32_t newStride = make_stride_aligned(4);
				std::vector<uint8_t> padding(newStride - m_rowStride);

				for (int32_t y = 0; y < m_infoHeader->height; ++y) {
					inp.read(reinterpret_cast<char*>(m_data->data() + y * m_rowStride), m_rowStride);
					inp.read(reinterpret_cast<char*>(padding.data()), padding.size());
				}
				//m_data->resize(m_infoHeader->height * byteCount * (m_infoHeader->width + 1), 0); //byte * H * [W  + 1]	for padding resolution
				m_fileHeader->file_size += m_data->size() + m_infoHeader->height * padding.size();
			}
			
			operationSucceeded = true;
		}
		else {
			std::cerr << "Unable to open this input image file." << std::endl;
		}
		
		inp.close();
		return operationSucceeded;
	}

	void Pixmap::write(const char* filename) 
	{
		std::ofstream of{ filename, std::ofstream::binary | std::ofstream::trunc };
		if (of) {
			if (m_infoHeader->bit_count == 32) {
				write_header_and_data(of);
			}
			else if (m_infoHeader->bit_count == 24) {
				if((m_infoHeader->width % 4) == 0){
					write_header_and_data(of);
				}
				else {
					uint32_t newStride = make_stride_aligned(4);
					std::vector<uint8_t> padding(newStride - m_rowStride);

					write_header(of);
					for (int32_t y = 0; y < m_infoHeader->height; ++y) {
						of.write(reinterpret_cast<const char*>(m_data->data() + y * m_rowStride), m_rowStride);
						of.write(reinterpret_cast<const char*>(padding.data()), padding.size());
					}
				}
			}
		}
		else {
			std::cerr << "Unable to open this input image file." << std::endl;
		}

		of.close();
	}

	uint32_t Pixmap::make_stride_aligned(uint32_t stride)
	{
		uint32_t newStride = m_rowStride;
		while ((newStride % 4) !=0)
			newStride++;

		return newStride;
	}

	void Pixmap::check_color_header(const std::unique_ptr<PixmapColorTable>& colorHeaderPtr) const 
	{
		PixmapColorTable expectedColorHeader{};

		if (colorHeaderPtr->alpha_mask != expectedColorHeader.alpha_mask
			|| colorHeaderPtr->red_mask != expectedColorHeader.red_mask
			|| colorHeaderPtr->green_mask != expectedColorHeader.green_mask
			|| colorHeaderPtr->blue_mask != expectedColorHeader.blue_mask) {
			
			throw std::runtime_error("");
		}

		if (colorHeaderPtr->color_space_type != expectedColorHeader.color_space_type) {
			throw std::runtime_error("");
		}
	}

	void Pixmap::write_header_and_data(std::ofstream& of) const
	{
		write_header(of);
		of.write(reinterpret_cast<const char*>(m_data->data()), m_data->size());
	}

	void Pixmap::write_header(std::ofstream& of) const
	{
		of.write(reinterpret_cast<const char*>(m_fileHeader.get()), sizeof(PixmapFile));
		of.write(reinterpret_cast<const char*>(m_infoHeader.get()), sizeof(PixmapInfo));
		if (m_infoHeader->bit_count == 32) {
			of.write(reinterpret_cast<const char*>(m_colorTableHeader.get()), sizeof(PixmapColorTable));
		}
	}

	void Pixmap::fill_region(
		uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
		uint8_t r, uint8_t g, uint8_t b, uint8_t a
	) 
	{
		if ((x0 + w) > static_cast<uint32_t>(m_infoHeader->width)
			|| (y0 + h) > static_cast<uint32_t>(m_infoHeader->height)) {
			throw std::runtime_error("");
		}

		int32_t channel = m_infoHeader->bit_count / 8;
		auto pixels = m_data->data();

		for (auto y = y0; y < y0 + h; ++y) {
			for (auto x = x0; x < x0 + w; ++x) {
				pixels[channel * (x + y * m_infoHeader->width) + 0] = b;
				pixels[channel * (x + y * m_infoHeader->width) + 1] = g;
				pixels[channel * (x + y * m_infoHeader->width) + 2] = r;

				if (channel == 4) {
					pixels[channel * (x + y * m_infoHeader->width) + 3] = a;
				}
			}
		}
	}

}