// Wave equation? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct wave_equation
{
	int x_res;
	int y_res;

	float* values;

	float* vx;
	float* vy;
};
