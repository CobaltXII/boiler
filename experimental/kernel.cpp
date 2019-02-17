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

kernel edge_detect_2()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 0.0f, 0.0f + 1.0f, 0.0f + 0.0f},
		{0.0f + 1.0f, 0.0f - 4.0f, 0.0f + 1.0f},
		{0.0f + 0.0f, 0.0f + 1.0f, 0.0f + 0.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

kernel edge_detect_3()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f - 1.0f, 0.0f - 1.0f, 0.0f - 1.0f},
		{0.0f - 1.0f, 0.0f + 8.0f, 0.0f - 1.0f},
		{0.0f - 1.0f, 0.0f - 1.0f, 0.0f - 1.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

kernel sharpen()
{
	int x = 3;

	kernel kern = {x, x, (float*)malloc(x * x * sizeof(float))};

	float k[3][3] =
	{
		{0.0f + 0.0f, 0.0f - 1.0f, 0.0f + 0.0f},
		{0.0f - 1.0f, 0.0f + 5.0f, 0.0f - 1.0f},
		{0.0f + 0.0f, 0.0f - 1.0f, 0.0f + 0.0f}
	};

	for (int i = 0; i < x; i++)
	for (int j = 0; j < x; j++)
	{
		kern.k[j * x + i] = k[j][i];
	}

	return kern;
}

// I wrote this one by translating the math.

float guassian_2d(float x, float y, float stdev)
{
	return 1.0f / (2.0f * M_PI * (stdev * stdev)) * pow(M_E, -(x * x + y * y) / (2.0f * (stdev * stdev)));
}

// However, most programmers seem to use this one.

float popular_guassian_2d(float x, float y, float stdev)
{
	return exp(-(x * x + y * y) / (2.0f * stdev * stdev)) / (2.0f * M_PI * stdev * stdev);
}
