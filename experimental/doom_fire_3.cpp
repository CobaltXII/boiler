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

struct point
{
	float x;
	float y;

	float ox;
	float oy;

	point(float _x, float _y)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;
	}

	point(float _x, float _y, float _ox, float _oy)
	{
		x = _x;
		y = _y;

		ox = _ox;
		oy = _oy;
	}
};

typedef unsigned char cell;

struct game: boiler
{	
	// Stuff to do with the Verlet balls simulation.

	std::vector<point*> points;

	float* selection_x = nullptr;
	float* selection_y = nullptr;

	const float r = 5.0f;

	// Stuff to do with the fire simulation.

	const int block_xr = 3;
	const int block_yr = 3;

	const int x_res = 600;
	const int y_res = 600;

	const int fx_res = x_res / block_xr;
	const int fy_res = y_res / block_yr;

	cell* fire_map = nullptr;

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else
		{
			points.push_back
			(
				new point
				(
					mouse_x / block_xr, 
					mouse_y / block_yr
				)
			);
		}
	}

	void steam() override
	{
		width = x_res;
		height = y_res;

		title = "Doom fire (using Boiler)";

		// Allocate fire_map.

		fire_map = (cell*)malloc(fx_res * fy_res * sizeof(cell));

		// Clear the fire_map.

		memset(fire_map, 0, fx_res * fy_res * sizeof(cell));

		// Add balls.

		srand(time(NULL));

		for (int i = 0; i < 4; i++)
		{
			points.push_back
			(
				new point
				(
					rand_11() * 10 + fx_res / 2,
					rand_11() * 10 + fy_res / 2
				)
			);
		}
	}

	void spread_fire(int idx)
	{
		// Spread fire upwards and horizontally while losing heat.

		int deviation = std::round(rand_01() * 2.4f);

		if (fire_map[idx] - (deviation & 1) * 3 >= 0)
		{
			fire_map[(idx - deviation + 1) - fx_res] = fire_map[idx] - (deviation & 1) * 3;
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