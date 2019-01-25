// Verlet sandbox? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef float real;

// Naive velocity clamp.

real clampm(real val, real clamp)
{
	if (val > 0.0f)
	{
		return std::min(val, clamp);
	}
	else
	{
		return std::max(val, -clamp);
	}
}

// A point.

struct point
{
	// Current position.

	real x;
	real y;

	// Previous position.

	real ox;
	real oy;

	// Locked points cannot move.

	bool locked;

	// Default constructor.

	point
	(
		real _x, 
		real _y, 

		bool _locked = false
	)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;

		locked = _locked;
	}
};
