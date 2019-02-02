/*

Doom fire? Awesome.

Thanks to Fabien Sanglard for his wonderful article:

    http://fabiensanglard.net/doom_fire_psx/index.html

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef unsigned char cell;

struct game: boiler
{	
	const int block_xr = 2;
	const int block_yr = 2;

	const int x_res = 600;
	const int y_res = 600;

	const int fx_res = x_res / block_xr;
	const int fy_res = y_res / block_yr;

	unsigned int color_map[37] =
	{
        rgb(0x07, 0x07, 0x07),
        rgb(0x1F, 0x07, 0x07),
        rgb(0x2F, 0x0F, 0x07),
        rgb(0x47, 0x0F, 0x07),
        rgb(0x57, 0x17, 0x07),
        rgb(0x67, 0x1F, 0x07),
        rgb(0x77, 0x1F, 0x07),
        rgb(0x8F, 0x27, 0x07),
        rgb(0x9F, 0x2F, 0x07),
        rgb(0xAF, 0x3F, 0x07),
        rgb(0xBF, 0x47, 0x07),
        rgb(0xC7, 0x47, 0x07),
        rgb(0xDF, 0x4F, 0x07),
        rgb(0xDF, 0x57, 0x07),
        rgb(0xDF, 0x57, 0x07),
        rgb(0xD7, 0x5F, 0x07),
        rgb(0xD7, 0x5F, 0x07),
        rgb(0xD7, 0x67, 0x0F),
        rgb(0xCF, 0x6F, 0x0F),
        rgb(0xCF, 0x77, 0x0F),
        rgb(0xCF, 0x7F, 0x0F),
        rgb(0xCF, 0x87, 0x17),
        rgb(0xC7, 0x87, 0x17),
        rgb(0xC7, 0x8F, 0x17),
        rgb(0xC7, 0x97, 0x1F),
        rgb(0xBF, 0x9F, 0x1F),
        rgb(0xBF, 0x9F, 0x1F),
        rgb(0xBF, 0xA7, 0x27),
        rgb(0xBF, 0xA7, 0x27),
        rgb(0xBF, 0xAF, 0x2F),
        rgb(0xB7, 0xAF, 0x2F),
        rgb(0xB7, 0xB7, 0x2F),
        rgb(0xB7, 0xB7, 0x37),
        rgb(0xCF, 0xCF, 0x6F),
        rgb(0xDF, 0xDF, 0x9F),
        rgb(0xEF, 0xEF, 0xC7),
        rgb(0xFF, 0xFF, 0xFF)
	};

	cell* fire_map = nullptr;

	void steam() override
	{
		width = x_res;
		height = y_res;

		title = "Doom fire (using Boiler)";

		// Ruin the palette.

		int palette_len = sizeof(color_map) / sizeof(color_map[0]);

		for (int i = 0; i < palette_len; i++)
		{
			unsigned char r = mgetr(color_map[i]);
			unsigned char g = mgetg(color_map[i]);
			unsigned char b = mgetb(color_map[i]);

			color_map[i] = rgb(r, g, b);
		}

		// Allocate fire_map.

		fire_map = (cell*)malloc(fx_res * fy_res * sizeof(cell));

		// Clear the fire_map.

		memset(fire_map, 0, fx_res * fy_res * sizeof(cell));
	}

	void spread_fire(int idx)
	{
		// Spread fire upwards and horizontally while losing heat.

		int deviation = std::round(rand_01() * 2.4f);

		if (fire_map[idx] - (deviation & 1) >= 0)
		{
			fire_map[(idx - deviation + 1) - fx_res] = fire_map[idx] - (deviation & 1);
		}
	}

	void draw() override
	{
		black();

		// Propagate the fire.

		for (int x = 0; x < fx_res; x++)
		{
			for (int y = 0; y < fy_res; y++)
			{
				spread_fire(y * fx_res + x);
			}
		}

		// Render the fire.

		for (int y = 0; y < fy_res; y++)
		{
			for (int x = 0; x < fx_res; x++)
			{
				frectrgb(x * block_xr, y * block_yr, block_xr, block_yr, color_map[fire_map[y * fx_res + x]]);
			}
		}

		// Allow the mouse to place fire.

		if (mouse_l)
		{
			fire_map[(mouse_y / block_yr) * fx_res + (mouse_x / block_xr)] = 36;
		}

		// Set the bottom row to full heat.

		for (int i = 0; i < fx_res; i++)
		{
			fire_map[(fy_res - 1) * fx_res + i] = 36;
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