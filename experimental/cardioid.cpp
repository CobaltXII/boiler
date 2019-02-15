// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	int r = 350;

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Emptyness (using Boiler)";
	}

	void draw() override
	{
		black();

		// Draw the circle to help with visualization.

		circlergb(h_width, h_height, r, rgb(255, 255, 255));

		// Draw n equally spaced segments around the circle.

		int n = 256;

		for (int i = 0; i < n; i++)
		{
			int x = cos(degrad(360.0f / float(n - 1) * i)) * r;
			int y = sin(degrad(360.0f / float(n - 1) * i)) * r;

			fcirclergb(x + h_width, y + h_height, 2, rgb(255, 255, 255));
		}

		// Draw the line segment from point i to point (i * 2) % n for each
		// value n > i >= 0.

		for (int i = 0; i < n; i++)
		{
			int x1 = cos(degrad(360.0f / float(n - 1) * i)) * r;
			int y1 = sin(degrad(360.0f / float(n - 1) * i)) * r;

			int x2 = cos(degrad(360.0f / float(n - 1) * ((i * 2) % n))) * r;
			int y2 = sin(degrad(360.0f / float(n - 1) * ((i * 2) % n))) * r;

			linergb(x1 + h_width, y1 + h_height, x2 + h_width, y2 + h_height, rgb(255, 255, 255));
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