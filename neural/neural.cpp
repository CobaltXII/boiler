// Neural networks? Awesome.

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <iostream>

#include "neural_matrix.h"
#include "neural_network.h"

struct game: boiler
{	
	void steam() override
	{
		width = 800;
		height = 600;

		title = "Neural network (using Boiler)";
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Visualize a two-dimensional function.

		double r_x = 256;
		double r_y = 256;

		int o_x = (int)h_width - (r_x / 2);
		int o_y = (int)h_height - (r_y / 2);

		for (int i = 0; i < r_x; i++)
		{
			for (int j = 0; j < r_y; j++)
			{
				double n_x = i / r_x;
				double n_y = j / r_y;

				double f = std::abs(n_x - n_y);

				pixels[(o_y + j) * width + (o_x + i)] = rgb(f * 256, f * 256, f * 256);
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