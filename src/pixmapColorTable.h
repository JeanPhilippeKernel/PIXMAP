#ifndef COLORTABLEHEADER_H_
#define COLORTABLEHEADER_H_


namespace imagelib 
{
#pragma pack(push, 1)
	struct PixmapColorTable
	{
		uint32_t	red_mask{ 0x00ff0000 };         // Bit mask for the red channel
		uint32_t	green_mask{ 0x0000ff00 };       // Bit mask for the green channel
		uint32_t	blue_mask{ 0x000000ff };        // Bit mask for the blue channel
		uint32_t	alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
		uint32_t	color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
		uint32_t	unused[16]{ 0 };                // Unused data for sRGB color space
	};
#pragma pack(pop)
}

#endif // !COLORTABLEHEADER_H_
