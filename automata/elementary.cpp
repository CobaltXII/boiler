// Elementary cellular automaton family? Awesome. Sometimes causes a
// segmentation fault, not sure why.

#include "../boiler/boiler.h"

#include <bitset>
#include <vector>
#include <utility>
#include <iostream>

typedef unsigned char cell;

bool* bit_array;

struct game: boiler
{	
	cell* state;

	unsigned int cell_color[2] =
	{
		rgb(0x00, 0x00, 0x00),
		rgb(0xFF, 0xFF, 0xFF)
	};

	void steam() override
	{
		width = 600;
		
		height = 600;

		title = "Elementary cellular automaton family (using Boiler)";

		// Seed the random number generator.

		srand(time(NULL));

		// Allocate state array.

		state = (cell*)malloc(width * height * sizeof(cell));

		// Initialize the top row with randomness.

		for (int i = 0; i < width; i++)
		{
			state[i] = rand() % 2;
		}

		// Fill the following horizontal lines using the ruleset.

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				cell cell_0 = state[(i * width) + (j + width - 1) % width];
				cell cell_1 = state[(i * width) + (j + width + 0) % width];
				cell cell_2 = state[(i * width) + (j + width + 1) % width];

				unsigned char permutation = 
				(
					(cell_2 << 0) + 
					(cell_1 << 1) + 
					(cell_0 << 2)
				);

				state[(i + 1) * width + j] = bit_array[7 - permutation];
			}
		}
	}

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_c)
		{
			if (cell_color[0])
			{
				cell_color[0] = rgb(0x00, 0x00, 0x00);
				cell_color[1] = rgb(0xFF, 0xFF, 0xFF);
			}
			else
			{
				cell_color[0] = rgb(0xFF, 0xFF, 0xFF);
				cell_color[1] = rgb(0x00, 0x00, 0x00);
			}
		}
		else if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
	}

	void draw() override
	{
		black();

		// Shift state1 up one pixel.

		memcpy(state, state + width * sizeof(cell), width * (height - 1) * sizeof(cell));

		// Use the ruleset to generate the next horizontal line.

		for (int i = 0; i < width; i++)
		{
			cell cell_0 = state[((height - 2) * width) + (i + width - 1) % width];
			cell cell_1 = state[((height - 2) * width) + (i + width + 0) % width];
			cell cell_2 = state[((height - 2) * width) + (i + width + 1) % width];

			unsigned char permutation = 
			(
				(cell_2 << 0) + 
				(cell_1 << 1) + 
				(cell_0 << 2)
			);

			state[(height - 1) * width + i] = bit_array[7 - permutation];
		}

		// Blit to the screen.

		for (int i = 0; i < width * height; i++)
		{
			pixels[i] = cell_color[state[i]];
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <rule>" << std::endl;

		exit(EXIT_FAILURE);
	}

	// Generate the rule.

	std::bitset<8> bits(atoi(argv[1]));

	bit_array = (bool*)malloc(8 * sizeof(bool));

	for (int i = 0; i < 8; i++)
	{
		bit_array[i] = bits[i];
	}

	// Start Boiler.

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