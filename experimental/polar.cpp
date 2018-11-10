// Polar coordinates? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	double x;
	double y;

	point(double _x = 0.0, double _y = 0.0)
	{
		x = _x;
		y = _y;
	}
};

struct game: boiler
{	
	// Convert from polar to Cartesian coordinates. X is rho and Y is theta.

	inline point polar_to_cartesian(point polar)
	{
		return point
		(
			polar.x * cos(polar.y),
			polar.x * sin(polar.y)
		);
	}

	// Draw a circle using polar coordinates.

	void polar_circle(double radius, unsigned int color)
	{
		double circ = 2.0 * M_PI * radius;

		double inc = 360.0 / circ;

		double deg = 0.0;

		for (int i = 0; i < circ; i++)
		{
			point polar = point(radius, degrad(deg));

			point cartesian = polar_to_cartesian(polar);

			plotp(cartesian.x + h_width, cartesian.y + h_height, color);

			deg += inc;
		}
	}

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Polar coordinates (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		for (double r = 6.0; r < 6.0 * 48.0; r += 6.0)
		{
			polar_circle(r, rgb(255, 255, 255));
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