// Image convolution using kernels? Awesome.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

std::string path_to_img;

struct kernel
{
	int kx;
	int ky;

	float* k;
};

kernel box_blur(int x)
{
	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = 1.0f / float(x * x);
	}

	return kern;
}

kernel edge_detect_1()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 1.0f, 0.0f + 0.0f, 0.0f - 1.0f},
		{0.0f + 0.0f, 0.0f + 0.0f, 0.0f + 0.0f},
		{0.0f - 1.0f, 0.0f + 0.0f, 0.0f + 1.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}
