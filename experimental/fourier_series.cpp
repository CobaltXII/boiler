// Fourier series? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct game: boiler
{	
	float cx;
	float cy;

	float cr;

	std::vector<float> line1;

	std::vector<float> line2x;
	std::vector<float> line2y;

	void steam() override
	{
		width = 1400;
		
		height = 400;

		title = "Fourier series (using Boiler)";

		cx = 364.0f;
		cy = 200.0f;

		cr = 96.0f;
	}

	void draw() override
	{
		black();

		float t = iteration / 30.0f;

		// Draw the fractal spirograph.

		float x = cx;
		float y = cy;

		for (int i = 0; i < 32; i++)
		{
			float px = x;
			float py = y;

    		float n = i * 2.0f + 1.0f;

    		float radius = 96.0f * (4.0f / (n * M_PI));

    		x += radius * cos(n * t);
    		y += radius * sin(n * t);

    		circlergb(px, py, radius, rgb(255, 255, 255));

    		linergb(px, py, x, y, rgb(0, 255, 0));
		}

		line1.push_back(y);

		// Add the position of the 'head' to line2x and line2y. Do this many 
		// times to create a smooth line.

		for (int i = 0; i < 8; i++)
		{
			float x = cx;
			float y = cy;

			float t = (iteration - 1.0f + i * (1.0f / 8.0f)) / 30.0f;

			for (int i = 0; i < 32; i++)
			{
				float px = x;
				float py = y;

	    		float n = i * 2.0f + 1.0f;

	    		float radius = 96.0f * (4.0f / (n * M_PI));

	    		x += radius * cos(n * t);
	    		y += radius * sin(n * t);
			}

			line2x.push_back(x);
			line2y.push_back(y);
		}

		// Draw a line connecting the 'head' of the fractal spirograph to the
		// start of line1, making it look like the 'head' is 'writing'.

		linergb(x, y, cx + cr + 256, y, rgb(255, 255, 255));

		// Draw line1 (the Y coordinate of the 'head' over time).

		for (int i = 1; i < line1.size(); i++)
		{
			linergb(cx + cr + 256 + i, line1[line1.size() - i], cx + cr + 256 + i + 1, line1[line1.size() - i - 1], rgb(255, 255, 255));
		}

		if (line1.size() > 1024)
		{
			line1.erase(line1.begin());
		}

		// Draw line2x and line2y (the path of the 'head' over time).

		for (int i = 0; i < line2x.size() - 1; i++)
		{
			linergb(line2x[i], line2y[i], line2x[i + 1], line2y[i + 1], rgb(255, 0, 0));
		}

		if (line2x.size() > 1024 * 16)
		{
			line2x.erase(line2x.begin());
			line2y.erase(line2y.begin());
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