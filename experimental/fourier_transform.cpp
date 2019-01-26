// Fourier transform? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <sstream>
#include <iostream>

struct fourier_t
{
	float re;
	float im;

	float freq;

	float amp;

	float phase;
};

// Discrete Fourier transform.

std::vector<fourier_t> discrete_fourier_transform(std::vector<float> x)
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

			re = re + x[int(n)] * cos(phi);
			im = im - x[int(n)] * sin(phi);
		}

		re = re / N;
		im = im / N;

		X[int(k)] = {re, im, k, sqrt(re * re + im * im), atan2(im, re)};
	}

	return X;
}
