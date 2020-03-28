#ifndef FILEHEADER_H_
#define FILEHEADER_H_

namespace imagelib
{
#pragma pack(push, 1)

	struct PixmapFile
	{
		uint16_t file_type{ 0x4D42 };          // File type always BM which is 0x4D42
		uint32_t file_size{ 0 };               // Size of the file (in bytes)
		uint16_t reserved1{ 0 };               // Reserved, always 0
		uint16_t reserved2{ 0 };               // Reserved, always 0
		uint32_t offset_data{ 0 };             // Start position of pixel data (bytes from the beginning of the file)
	};
#pragma pack(pop)
}


#endif // !FILEHEADER_H_
