// Quadtrees? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	double x;
	double y;

	point(double _x = 0.0, double _y = 0.0)
	{
		x = _x;
		y = _y;
	}
};
