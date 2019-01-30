// Cellular automata? Awesome.

#define BOIL_PRE_INCLUDE

#include <SDL.h>

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef unsigned char cell;

struct game: boiler
{	
	cell* state1;
	cell* state2;

	unsigned int state_len;

	unsigned int cell_color[2] =
	{
		rgb(0x00, 0x00, 0x00),
		rgb(0xFF, 0xFF, 0xFF)
	};

	void steam() override
	{
		width = 600;
		
		height = 600;

		title = "Cellular automata (using Boiler)";

		// Allocate states.

		state_len = width * height;

		state1 = (cell*)malloc(state_len * sizeof(cell));
		state2 = (cell*)malloc(state_len * sizeof(cell));

		// Initialize to randomness.

		for (int i = 0; i < state_len; i++)
		{
			state1[i] = 1 - std::min(1, rand() % 1024);
		}
	}

	void draw() override
	{
		black();

		// Set at mouse pointer.

		if (mouse_l)
		{
			state1[mouse_y * width + mouse_x] = 1;
		}

		// Do life operations.

		#pragma omp parallel for schedule(dynamic)

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				// Get current state.

				cell c = state1[y * width + x];

				// Get neighbors.

				cell tl = state1[((y + height - 1) % height) * width + ((x + width - 1) % width)];
				cell tc = state1[((y + height - 1) % height) * width + ((x + width + 0) % width)];
				cell tr = state1[((y + height - 1) % height) * width + ((x + width + 1) % width)];

				cell bl = state1[((y + height + 1) % height) * width + ((x + width - 1) % width)];
				cell bc = state1[((y + height + 1) % height) * width + ((x + width + 0) % width)];
				cell br = state1[((y + height + 1) % height) * width + ((x + width + 1) % width)];

				cell cl = state1[y * width + ((x + width - 1) % width)];
				cell cr = state1[y * width + ((x + width + 1) % width)];

				// Count live neighbors.

				unsigned int neighbors = tl + tc + tr + bl + bc + br + cl + cr;

				// Check the cell's life status.

				if (c)
				{
					state2[y * width + x] = 0;
				}
				else
				{
					if (neighbors == 2)
					{
						state2[y * width + x] = 1;
					}
				}
			}
		}

		// Move state2's contents to state1.

		memcpy(state1, state2, state_len * sizeof(cell));

		// Blit to the screen.

		for (int i = 0; i < state_len; i++)
		{
			pixels[i] = cell_color[state1[i]];
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