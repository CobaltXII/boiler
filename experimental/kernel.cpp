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
