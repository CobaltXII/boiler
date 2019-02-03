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