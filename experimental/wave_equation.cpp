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

	inline void add_wave(int x, int y, int r, float amplitude)
	{
		// Thanks, Eriksonn.
		
    	for (int x2 = x - r; x2 <= x + r; x2++)
    	for (int y2 = y - r; y2 <= y + r; y2++)
    	{
    		int x3 = x2 - x;
    		int y3 = y2 - y;

    		float radius = sqrt(x3 * x3 + y3 * y3) / float(r);

    		if (radius < 1.0f)
    		{
    			add_value(x2, y2, (cosf(radius * M_PI) + 1.0f) * amplitude);
    		}
    	}
	}
};

struct game: boiler
{	
	int x_res = 200;
	int y_res = 200;

	int scale = 4;

	wave_equation simulation = wave_equation(x_res, y_res);

	void steam() override
	{
		width = x_res * scale;
		
		height = y_res * scale;

		title = "Wave equation (using Boiler)";
	}

	void draw() override
	{
		black();

		if (mouse_l)
		{
			int x = mouse_x / scale;
			int y = mouse_y / scale;

			simulation.add_wave(x, y, 10, 0.5f);
		}

		simulation.step(0.5f, 1.0f);

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			float value = (simulation.values[simulation.idx(i, j)] + 1.0f) / 2.0f;

			if (value < 0.5f)
			{
				float b = mclamprgb((0.5f - value) * 512.0f);
				
				frectrgb(i * scale, j * scale, scale, scale, rgb(0, 0, b));
			}
			else
			{
				float r = mclamprgb((value - 0.5f) * 512.0f);

				frectrgb(i * scale, j * scale, scale, scale, rgb(r, 0, 0));
			}
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	game demo;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}