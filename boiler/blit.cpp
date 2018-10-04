// Blitting demonstration.

#include "boiler.h"

struct game: boiler
{	
	int lena_w;
	int lena_h;

	Uint32* lena = loadbmp("lena_color.bmp", lena_w, lena_h);

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Software renderer (using Boiler)";
	}

	void draw() override
	{
		clear(rgb(255, 255, 255));

		if (lena)
		{
			blitrgb(lena, 0, 0, lena_w, lena_h, mouse_x - lena_w / 2, mouse_y - lena_h / 2, lena_w, lena_h); 
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	game demo;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}