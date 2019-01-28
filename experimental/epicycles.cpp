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
