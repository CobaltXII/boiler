// Old school fire effect? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

const int cw = 4;
const int ch = 4;

const int w = 800 / cw;
const int h = 600 / ch;

struct game: boiler
{	
	Uint32 fire[h][w];

	Uint32 palette[256];

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Old school fire effect (using Boiler)";

		// Generate palette.

		for (int i = 0; i < 256; i++)
		{
			palette[i] = hsltorgb(i / 3, 1.0, std::min(255, i * 2) / 255.0);
		}

		// Nullify fire buffer.

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				fire[y][x] = 0;
			}
		}
	}

	void draw() override
	{
		// Set to true for rainbow palette!

		if (false)
		{
			// Generate palette.

			for (int i = 0; i < 256; i++)
			{
				palette[i] = hsltorgb(i / 3 + iteration, 1.0, std::min(255, i * 2) / 255.0);
			}
		}

		// Randomize bottom.

		for (int x = 0; x < w; x++)
		{
			fire[h - 1][x] = rand() % 256;
		}

		// Fire calculations.

		for (int y = 0; y < h - 1; y++)
		{
			for (int x = 0; x < w; x++)
			{
				// How on earth did people invent this? It's cellular automata, but the rules must
				// have taken a lot of thought.

				fire[y][x] = ((fire[(y + 1) % h][(x - 1 + w) % w] + fire[(y + 1) % h][x % w] + fire[(y + 1) % h][(x + 1) % w] + fire[(y + 2) % h][x % w]) * 32) / 129;
			}
		}

		// Draw to screen.

		for (int x = 0; x < w; x++)
		{
			for (int y = 0; y < h; y++)
			{
				frectrgb
				(
					x * cw,
					y * ch,

					cw,
					ch,

					palette[Uint8(fire[y][x])]
				);
			}
		}

		// Mouse drawing.

		if (mouse_l)
		{
			int brush = 16;

			for (int x = 0; x < brush; x++)
			{
				for (int y = 0; y < brush; y++)
				{
					int xpos = ((mouse_x / cw) + w - (brush / 2) + x) % w;
					int ypos = ((mouse_y / ch) + h - (brush / 2) + y) % h;

					fire[ypos][xpos] = rand() % 256;
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