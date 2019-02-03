/*

Doom fire? Awesome.

Thanks to Fabien Sanglard for his wonderful article:

    http://fabiensanglard.net/doom_fire_psx/index.html

Coloring algorithm from:

	https://adasba.github.io/adasba-portfolio-site/cellular_automata/life_on_fire.html

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

	cell* fire_map = nullptr;

	void steam() override
	{
		width = x_res;
		height = y_res;

		title = "Doom fire (using Boiler)";

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
				int r = fire_map[y * fx_res + x] * 14;

				int g = fire_map[y * fx_res + x] * 14 - 256;
				int b = fire_map[y * fx_res + x] * 28 - 768;

				unsigned int color = rgb
				(
					clamprgb(r),
					clamprgb(g),
					clamprgb(b)
				);

				frectrgb(x * block_xr, y * block_yr, block_xr, block_yr, color);
			}
		}

		// Allow the mouse to place fire.

		if (mouse_l)
		{
			int x = (mouse_x / block_xr);
			int y = (mouse_y / block_yr);

			for (int i = -4; i <= 8; i++)
			for (int j = -4; j <= 8; j++)
			{
				if (i * i + j * j < 8 * 8)
				{
					if (x + i >= 0 && x + i < fx_res)
					if (y + j >= 0 && y + j < fy_res)
					{
						fire_map[(y + j) * fx_res + (x + i)] = 36;
					}
				}
			}
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