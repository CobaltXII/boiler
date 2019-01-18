#pragma once

// A two-dimensional point.

struct point
{
	real x;
	real y;

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}

	point(real _x, real _y)
	{
		x = _x;
		y = _y;
	}
};
