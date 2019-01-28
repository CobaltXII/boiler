// K-means clustering? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

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
