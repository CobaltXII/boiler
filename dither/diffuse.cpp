#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <random>
#include <vector>
#include <utility>
#include <iostream>

std::string path_to_img;

// An image is a one-dimensional array of unsigned integers. However, a one-dimensional array of
// unsigned integers can be any number of other things as well. For clarity, we use a typedef.

typedef Uint32* image_rgb;

// Similar to the above statement, a grayscale image can be represented by a one-dimensional array
// of unsigned chars. An array of unsigned chars is ambiguous, so we use a typedef.

typedef Uint8* image_gs;

// A three-dimensional point.

struct point
{
	float x;
	float y;
	float z;

	point()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	point(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};
