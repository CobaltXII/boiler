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
