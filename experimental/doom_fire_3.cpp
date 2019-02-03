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