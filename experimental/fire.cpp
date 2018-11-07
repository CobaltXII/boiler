// Old school fire effect? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

const int cw = 4;
const int ch = 4;

const int w = 800 / cw;
const int h = 600 / ch;

double huetorgb(double v1, double v2, double vh)
{
	if (vh < 0.0)
	{
		vh += 1.0;
	}

	if (vh > 1.0)
	{
		vh -= 1.0;
	}

	if ((6.0 * vh) < 1.0)
	{
		return (v1 + (v2 - v1) * 6.0 * vh);
	}

	if ((2.0 * vh) < 1.0)
	{
		return v2;
	}

	if ((3.0 * vh) < 2.0)
	{
		return (v1 + (v2 - v1) * ((2.0 / 3.0) - vh) * 6);
	}

	return v1;
}

unsigned int hsltorgb(int h, double s, double l)
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;

	if (s == 0)
	{
		r = (unsigned char)(l * 255);
		g = (unsigned char)(l * 255);
		b = (unsigned char)(l * 255);
	}
	else
	{
		double v1;
		double v2;

		double hue = h / 360.0;

		if (l < 0.5)
		{
			v2 = l * (1.0 + s);
		}
		else
		{
			v2 = (l + s) - (l * s);
		}

		v1 = 2.0 * l - v2;

		r = (unsigned char)(255 * huetorgb(v1, v2, hue + (1.0 / 3.0)));

		g = (unsigned char)(255 * huetorgb(v1, v2, hue));

		b = (unsigned char)(255 * huetorgb(v1, v2, hue - (1.0 / 3.0)));
	}

	return rgb(r, g, b);
}

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