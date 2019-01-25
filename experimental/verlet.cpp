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
