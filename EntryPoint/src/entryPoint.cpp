#include <iostream>


#include "pixmap.h"


using namespace imagelib;

int main(int argc, const char** argv)
{
	Pixmap pix{ 800, 600 };

	pix.fill_region(0, 0, 400, 200, 233, 200, 23, 255);
	pix.write("assets/generated.bmp");

	return EXIT_SUCCESS;
}