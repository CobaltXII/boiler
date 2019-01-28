// Fourier transform? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <sstream>
#include <iostream>

// Fourier transform output structure.

struct fourier_t
{
	float re;
	float im;

	float freq;

	float amp;

	float phase;
};

// A point.

struct point
{
	float x;
	float y;

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}

	point(float _x, float _y)
	{
		x = _x;
		y = _y;
	}
};

// Discrete Fourier transform.

std::vector<fourier_t> discrete_fourier_transform(std::vector<point>& x)
{
	float N = x.size();

	std::vector<fourier_t> X(N);

	for (float k = 0.0f; k < N; k++)
	{
		float re = 0.0f;
		float im = 0.0f;

		for (float n = 0.0f; n < N; n++)
		{
			float phi = (M_PI * 2.0f * k * n) / N;

			re = re + x[int(n)].x * cos(phi) + x[int(n)].y * sin(phi);
			im = im - x[int(n)].x * sin(phi) + x[int(n)].y * cos(phi);
		}

		re = re / N;
		im = im / N;

		X[int(k)] = {re, im, k, sqrt(re * re + im * im), atan2(im, re)};
	}

	return X;
}

struct game: boiler
{	
	float cx = 0.0f;
	float cy = 0.0f;

	std::vector<point> input;

	std::vector<float> linex;
	std::vector<float> liney;

	std::vector<fourier_t> fourier;

	// A state of 0 means that the system is waiting for input. A state of 1
	// means that the system is in the process of capturing input. A state of
	// 2 means that the system is normal.

	int state = 0;

	void steam() override
	{
		width = 800;
		height = 800;

		title = "Fourier transform (using Boiler)";

		// Find the center.

		cx = 0.5f * width;
		cy = 0.5f * height;

		std::cout << "Press the mouse down to begin drawing a continuous path. Once you release the mouse, the epicycles will draw your path back to you." << std::endl;
	}

	void draw() override
	{
		black();

		if (state == 0)
		{
			if (mouse_l)
			{
				// Mouse was pressed, begin capturing input.

				state = 1;
			}
		}
		else if (state == 1)
		{
			if (mouse_l)
			{
				// Mouse is still being pressed, continue capturing input. 
				// Store this input in the input vector.

				input.push_back(point(mouse_x, mouse_y));
			}
			else
			{
				// Mouse was released, conclude capturing input. Do the 
				// Fourier transform on the captured input.

				state = 2;

				std::vector<point> fourier_input;

				for (int i = 0; i < input.size(); i++)
				{
					point datum;

					datum.x = input[i].x - cx;
					datum.y = input[i].y - cy;

					fourier_input.push_back(datum);
				}

				fourier = discrete_fourier_transform(fourier_input);
			}

			// Draw the currently captured input.

			for (int i = 0; i < input.size() - 1; i++)
			{
				linergb
				(
					input[i].x,
					input[i].y,

					input[i + 1].x,
					input[i + 1].y,

					rgb(255, 255, 255)
				);
			}
		}
		else if (state == 2)
		{
			// Get the time.

			float t = iteration * (2.0f * M_PI) / float(fourier.size());

			// Draw the fractal spirograph.

			float x = cx;
			float y = cy;

			for (int i = 0; i < fourier.size(); i++)
			{
				float px = x;
				float py = y;

	    		float radius = fourier[i].amp;

	    		x += radius * cos(fourier[i].freq * t + fourier[i].phase);
	    		y += radius * sin(fourier[i].freq * t + fourier[i].phase);

	    		circlergb(px, py, radius, rgb(63, 63, 63));

	    		linergb(px, py, x, y, rgb(127, 127, 127));
			}

			// Draw linex and liney.

			linex.push_back(x);
			liney.push_back(y);

			for (int i = 0; i < linex.size() - 1; i++)
			{
				linergb(linex[i], liney[i], linex[i + 1], liney[i + 1], rgb(255, 255, 255));
			}

			if (linex.size() > fourier.size() * 16)
			{
				linex.erase(linex.begin());
				liney.erase(liney.begin());
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