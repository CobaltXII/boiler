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