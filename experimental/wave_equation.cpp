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

	wave_equation(int x_res, int y_res)
	{
		this->x_res = x_res;
		this->y_res = y_res;

		values = (float*)malloc(x_res * y_res * sizeof(float));

		vx = (float*)malloc(x_res * y_res * sizeof(float));
		vy = (float*)malloc(x_res * y_res * sizeof(float));
	}

	inline int idx(int x, int y)
	{
		x = std::max(0, std::min(x_res - 1, x));
		y = std::max(0, std::min(y_res - 1, y));

		return y * x_res + x;
	}
};
