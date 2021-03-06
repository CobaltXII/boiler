// Emptyness? Awesome.

#define BOIL_PRE_INCLUDE

#include <SDL.h>

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef float real;

struct body
{
	real x;
	real y;

	real vx;
	real vy;

	body(real _x, real _y, real _vx = 0.0f, real _vy = 0.0f)
	{
		x = _x;
		y = _y;

		vx = _vx;
		vy = _vy;
	}
};

void body_force(std::vector<body>& bodies, real timestep)
{
	const real softening = 1024.0f * 4.0f;

	#pragma omp parallel for schedule(dynamic)

	for (int i = 0; i < bodies.size(); i++)
	{
		real fx = 0.0f;
		real fy = 0.0f;

		for (int j = 0; j < bodies.size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			real dx = bodies[j].x - bodies[i].x;
			real dy = bodies[j].y - bodies[i].y;

			real dist_sqr = dx * dx + dy * dy;

			if (dist_sqr < softening)
			{
				dist_sqr = softening;
			}

			real inv_dist = 1.0f / sqrtf(dist_sqr);

			real inv_dist_3 = 
			(
				inv_dist *
				inv_dist *
				inv_dist
			);

			fx += dx * inv_dist_3;
			fy += dy * inv_dist_3;
		}

		bodies[i].vx += timestep * fx;
		bodies[i].vy += timestep * fy;
	}
}

struct game: boiler
{	
	bool temporal_filtering = false;
	bool additive_filtering = false;

	std::vector<body> bodies;

	void steam() override
	{
		width = 512;
		
		height = 512;

		title = "Emptyness (using Boiler)";

		srand(1419);

		const real xr = 6000.0f;
		const real yr = 6000.0f;

		for (int i = 0; i < 8000; i++)
		{
			if (true)
			{
				// Circular placement.

				real r1 = real(rand()) / real(RAND_MAX) * 2.0f - 1.0f;
				real r2 = real(rand()) / real(RAND_MAX) * 2.0f - 1.0f;
				
				real angx = cos(r1 * 2.0f * M_PI);
				real angy = sin(r1 * 2.0f * M_PI);

				real x = angx * r2 * xr;
				real y = angy * r2 * yr;

				real vx = cos(r1 * 2.0f * M_PI + degrad(45.0f)) * r2 * 32.0f;
				real vy = sin(r1 * 2.0f * M_PI + degrad(45.0f)) * r2 * 32.0f;

				bodies.push_back(body(x, y, vx, vy));
			}
			else
			{
				// Rectangular placement.

				real x = rand_11() * xr;
				real y = rand_11() * yr;

				bodies.push_back(body(x, y));
			}
		}

		additive_filtering = true;
	}

	void draw() override
	{
		if (temporal_filtering)
		{
			for (int i = 0; i < width * height; i++)
			{
				unsigned char n = mgetr(pixels[i]) / 1.125f;

				pixels[i] = rgb(n, n, n);
			}
		}
		else
		{
			black();
		}

		body_force(bodies, 800.0f);

		real inv_scale = 32.0f;

		for (int i = 0; i < bodies.size(); i++)
		{
			body& bi = bodies[i];

			bi.x += bi.vx;
			bi.y += bi.vy;

			if (additive_filtering)
			{
				int x = bi.x / inv_scale + h_width;
				int y = bi.y / inv_scale + h_height;

				if (x >= 0 && y >= 0 && x < width && y < height)
				{
					unsigned char low = mclamprgb(mgetr(pixels[y * width + x]) + 64);

					unsigned char r = low;
					unsigned char g = low;
					unsigned char b = low;

					plotp(bi.x / inv_scale + h_width, bi.y / inv_scale + h_height, rgb(r, g, b));
				}
			}
			else
			{
				plotp(bi.x / inv_scale + h_width, bi.y / inv_scale + h_height, rgb(255, 255, 255));
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