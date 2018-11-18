// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <random>
#include <utility>
#include <iostream>

#define mrandv ((double)((rand() % 65536) - (65536 / 2)) / (double)(65536 / 2))

struct point
{
	double x;
	double y;

	double vx;
	double vy;

	unsigned int c;

	point(double _x, double _y, double _vx, double _vy, unsigned int _c)
	{
		x = _x;
		y = _y;

		vx = _vx;
	 	vy = _vy;

		c = _c;
	}
};

struct game: boiler
{	
	std::vector<point> points;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Voronoi diagrams (using Boiler)";

		// Add balls.

		std::default_random_engine generator;

		std::uniform_int_distribution<int> distribution(0, 360);

		for (int i = 0; i < 64; i++)
		{
			points.push_back(point(mrandx, mrandy, mrandv * 2.0, mrandv * 2.0, hsl_to_rgb(distribution(generator), 0.45, 0.5)));
		}
	}

	void draw() override
	{
		black();

		// Move and collide points.

		for (int i = 0; i < points.size(); i++)
		{
			point& b = points[i];

			if (b.x > width)
			{
				b.x -= b.x - width;

				b.vx = -b.vx;
			}

			if (b.x < 0.0)
			{
				b.x += 0.0 - (b.x);

				b.vx = -b.vx;
			}

			if (b.y > height)
			{
				b.y -= b.y - height;

				b.vy = -b.vy;
			}

			if (b.y < 0.0)
			{
				b.y += 0.0 - (b.y);

				b.vy = -b.vy;
			}

			b.x += b.vx;
			b.y += b.vy;

			// circlergb(b.x, b.y, b.r, rgb(255, 255, 255));
		}

		// Voronoi diagram.

		int sx = 4;
		int sy = 4;

		for (int x = 0; x < width; x += sx)
		{
			for (int y = 0; y < height; y += sy)
			{
				unsigned int c_c = 0;

				double c_d = INFINITY;

				for (int i = 0; i < points.size(); i++)
				{
					point& b = points[i];

					double dx = x - b.x;
					double dy = y - b.y;

					double d = dx * dx + dy * dy;

					if (d < c_d)
					{
						c_c = b.c;

						c_d = d;
					}
				}

				frectrgb(x, y, sx, sy, c_c);
			}
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