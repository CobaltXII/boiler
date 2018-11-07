/*

Perlin noise flow fields.

*/

#include "../boiler/boiler.h"

#include "perlin.hpp"

#include <vector>
#include <utility>
#include <iostream>

struct particle
{
	double x;
	double y;

	double vx = 0.0;
	double vy = 0.0;

	particle(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
};

struct game: boiler
{	
	std::vector<particle> particles;

	int xlod = 5;
	int ylod = 5;

	int wlod;
	int hlod;

	double* xd;
	double* yd;

	void steam() override
	{
		width = 800;
		height = 600;

		f_No_Debug = SDL_TRUE;

		title = "Perlin noise flow field (using Boiler)";

		wlod = width / xlod;
		hlod = height / ylod;

		// Allocate direction buffers.

		xd = (double*)malloc(wlod * hlod * sizeof(double));
		yd = (double*)malloc(wlod * hlod * sizeof(double));

		if (xd == NULL || yd == NULL)
		{
			nuke("Could not allocate direction buffers!");
		}

		// Initialize particles.

		for (int i = 0; i < 100000; i++)
		{
			particles.push_back(particle(randx(), randy()));
		}
	}

	void draw() override
	{
		int fade = 255;

		for (int i = 0; i < width * height; i++)
		{
			unsigned int c = pixels[i];

			int r = mgetr(c);
			int g = mgetg(c);
			int b = mgetb(c);

			pixels[i] = mrgb
			(
				r - fade < 0 ? 0 : r - fade, 
				g - fade < 0 ? 0 : g - fade,
				b - fade < 0 ? 0 : b - fade
			);
		}

		if (iteration == 0)
		{
			double pmax = 0.0;
			double pmin = 1.0;

			double frequency = 32.0;

			int octaves = 16;

			perlin noise = perlin(time(NULL));

			double fx = hlod / frequency;
			double fy = hlod / frequency;

			for (int y = 0; y < hlod; y++)
			{
				for (int x = 0; x < wlod; x++)
				{
					double value = noise.octave_noise(x / fx, y / fy, octaves);

					if (value < pmin)
					{
						pmin = value;
					}

					if (value > pmax)
					{
						pmax = value;
					}

					double rad = degrad(value * 360.0);

					xd[y * wlod + x] = cos(rad);
					yd[y * wlod + x] = sin(rad);
				}
			}

			std::cout << "Minimum: " << pmin << std::endl;
			std::cout << "Maximum: " << pmax << std::endl;

			return;
		}

		// Render flow field (optional).

		for (int x = 0; x < wlod; x++)
		{
			for (int y = 0; y < hlod; y++)
			{
				continue;

				int cx = x * xlod + (xlod / 2);
				int cy = y * ylod + (ylod / 2);

				linergb
				(
					cx,
					cy,

					cx + xd[y * wlod + x] * xlod,
					cy + yd[y * wlod + x] * ylod,

					rgb(47, 47, 47)
				);
			}
		}

		// Render particles.

		for (int i = 0; i < particles.size(); i++)
		{
			particle& p_i = particles[i];

			// Apply flow field.

			int x = p_i.x / xlod;
			int y = p_i.y / ylod;

			double scalar = double(rand()) / double(RAND_MAX);

			p_i.vx += xd[y * wlod + x] * scalar;
			p_i.vy += yd[y * wlod + x] * scalar;

			p_i.vx += (mouse_x - p_i.x) / 10000.0;
			p_i.vy += (mouse_y - p_i.y) / 10000.0; 

			// Velocity constraint.

			p_i.vx *= 0.925;
			p_i.vy *= 0.925;

			// Apply velocity.

			p_i.x += p_i.vx;
			p_i.y += p_i.vy;

			// Boundary constraint.

			if (p_i.x > width - 1)
			{
				p_i.x = 0;
			}
			else if (p_i.x < 0)
			{
				p_i.x = width - 1;
			}

			if (p_i.y > height - 1)
			{
				p_i.y = 0;
			}
			else if (p_i.y < 0)
			{
				p_i.y = height - 1;
			}

			pixels[int(p_i.y) * width + int(p_i.x)] = rgb(255, 255, 255);
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

	free(demo.xd);
	free(demo.yd);

	return 0;
}