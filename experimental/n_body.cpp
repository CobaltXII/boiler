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

	body(real _x, real _y)
	{
		x = _x;
		y = _y;

		vx = 0.0f;
		vy = 0.0f;
	}
};

void body_force(std::vector<body>& bodies, real timestep)
{
	const real softening = 10.0f;

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

			real dist_sqr = dx * dx + dy * dy + softening;

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
	std::vector<body> bodies;

	void steam() override
	{
		width = 800;
		
		height = 600;

		title = "Emptyness (using Boiler)";

		const real xr = 1000.0f;
		const real yr = 1000.0f;

		for (int i = 0; i < 1000; i++)
		{
			real r1 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
			real r2 = float(rand()) / float(RAND_MAX) * 2.0f - 1.0f;
			
			real angx = cos(r1 * 2.0f * M_PI);
			real angy = sin(r1 * 2.0f * M_PI);

			bodies.push_back(body(angx * r2 * xr, angy * r2 * yr));
		}
	}

	bool temporal_filtering = false;

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

		body_force(bodies, 10.0f);

		for (int i = 0; i < bodies.size(); i++)
		{
			body& bi = bodies[i];

			bi.x += bi.vx;
			bi.y += bi.vy;

			plotp(bi.x / 10.0f + h_width, bi.y / 10.0f + h_height, rgb(255, 255, 255));
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