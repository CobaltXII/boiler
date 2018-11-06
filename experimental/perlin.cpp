#include "../boiler/boiler.h"

#include "perlin.hpp"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	unsigned int land_col(double n)
	{
		unsigned char x = n * 255;

		return rgb(x, x, x);
	}

	void steam() override
	{
		width = 800;
		height = 600;

		f_No_Debug = SDL_TRUE;

		title = "Perlin noise (using Boiler)";
	}

	void draw() override
	{
		if (iteration == 0)
		{
			double frequency = 8.0;

			int octaves = 16.0;

			perlin noise = perlin(time(NULL));

			double fx = height / frequency;
			double fy = height / frequency;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					double value = noise.octave_noise_0_1(x / fx, y / fy, octaves);

					value = pow(value, 1.1);

					pixels[y * width + x] = land_col(value);
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