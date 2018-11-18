// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#define mrandv ((double)((rand() % 65536) - (65536 / 2)) / (double)(65536 / 2))

unsigned int palette[7] = {rgb(255, 0, 0), rgb(0, 255, 0), rgb(0, 0, 255), rgb(255, 255, 0), rgb(255, 0, 255), rgb(0, 255, 255), rgb(255, 255, 255)};

struct ball
{
	double x;
	double y;

	double vx;
	double vy;

	double r;

	unsigned int c;

	ball(double _x, double _y, double _vx, double _vy, double _r, unsigned int _c)
	{
		x = _x;
		y = _y;

		vx = _vx;
	 	vy = _vy;

		r = _r;

		c = _c;
	}
};

struct game: boiler
{	
	std::vector<ball> balls;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Emptyness (using Boiler)";

		// Add balls.

		for (int i = 0; i < 32; i++)
		{
			balls.push_back(ball(mrandx, mrandy, mrandv * 2.0, mrandv * 2.0, rand() % 32 + 32, palette[rand() % 7]));
		}
	}

	void draw() override
	{
		black();

		// Move and collide balls.

		for (int i = 0; i < balls.size(); i++)
		{
			ball& b = balls[i];

			if (b.x + b.r > width)
			{
				b.x -= b.x + b.r - width;

				b.vx = -b.vx;
			}

			if (b.x - b.r < 0.0)
			{
				b.x += 0.0 - (b.x - b.r);

				b.vx = -b.vx;
			}

			if (b.y + b.r > height)
			{
				b.y -= b.y + b.r - height;

				b.vy = -b.vy;
			}

			if (b.y - b.r < 0.0)
			{
				b.y += 0.0 - (b.y - b.r);

				b.vy = -b.vy;
			}

			b.x += b.vx;
			b.y += b.vy;

			// circlergb(b.x, b.y, b.r, rgb(255, 255, 255));
		}

		// Metaballs.

		int sx = 4;
		int sy = 4;

		for (int x = 0; x < width; x += sx)
		{
			for (int y = 0; y < height; y += sy)
			{
				double sum = 0.0;

				double cr = 0.0;
				double cg = 0.0;
				double cb = 0.0;

				double w = 0.0;

				for (int i = 0; i < balls.size(); i++)
				{
					ball& b = balls[i];

					double dx = x - b.x;
					double dy = y - b.y;

					double d = dx * dx + dy * dy;

					double iw = 1.0 / (d + 1000.0);

					cr += double(mgetr(b.c)) * iw;
					cg += double(mgetg(b.c)) * iw;
					cb += double(mgetb(b.c)) * iw;

					w += iw;

					sum += b.r * b.r / d;
				}

				if (sum > 5.0)
				{
					cr /= w;
					cg /= w;
					cb /= w;

					frectrgb(x, y, sx, sy, rgb(cr, cg, cb));
				}
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