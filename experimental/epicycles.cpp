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
