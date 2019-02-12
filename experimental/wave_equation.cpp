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

	inline void step(float timestep, float diffuse)
	{
		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			vx[idx(i, j)] += (values[idx(i, j)] - values[idx(i + 1, j)]) * timestep;

			vy[idx(i, j)] += (values[idx(i, j)] - values[idx(i, j + 1)]) * timestep;
		}

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			values[idx(i, j)] -= vx[idx(i, j)] + vy[idx(i, j)];

			values[idx(i + 1, j)] += vx[idx(i, j)];

			values[idx(i, j + 1)] += vy[idx(i, j)];
		}

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			values[idx(i, j)] *= diffuse;
		}
	}

	inline void add_value(int x, int y, float value)
	{
		if (x < 0 || y < 0 || x >= x_res || y >= y_res)
		{
			return;
		}

		values[idx(x, y)] += value;
	}

	inline void add_velocity(int x, int y, float velocity_x, float velocity_y)
	{
		if (x < 0 || y < 0 || x >= x_res || y >= y_res)
		{
			return;
		}

		vx[idx(x, y)] += velocity_x;
		vy[idx(x, y)] += velocity_y;
	}
};
