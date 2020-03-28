#ifndef PIXMAPINFO_H_
#define PIXMAPINFO_H_

#include <cstdint>

namespace imagelib
{
#pragma pack(push, 1)

	struct PixmapInfo
	{
        uint32_t    size{ 0 };                          // Size of this header (in bytes)
        int32_t     width{ 0 };                         // width of bitmap in pixels
        int32_t     height{ 0 };                        // width of bitmap in pixels
                                                        //       (if positive, bottom-up, with origin in lower left corner)
                                                        //       (if negative, top-down, with origin in upper left corner)
        uint16_t    planes{ 1 };                        // No. of planes for the target device, this is always 1
        uint16_t    bit_count{ 0 };                     // No. of bits per pixel
        uint32_t    compression{ 0 };                   // 0 or 3 - uncompressed. THIS PROGRAM CONSIDERS ONLY UNCOMPRESSED BMP images
        uint32_t    size_image{ 0 };                    // 0 - for uncompressed images
        int32_t     x_pixels_per_meter{ 0 };
        int32_t     y_pixels_per_meter{ 0 };
        uint32_t    colors_used{ 0 };                   // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
        uint32_t    colors_important{ 0 };              // No. of colors used for displaying the bitmap. If 0 all colors are required
	};

#pragma pack(pop)

}


#endif // !PIXMAPINFO_H_
