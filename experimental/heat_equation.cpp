// Heat equation? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#include "jet.h"

struct heat_equation
{
	int x_res;
	int y_res;

	float* values;

	heat_equation(int x_res, int y_res)
	{
		this->x_res = x_res;
		this->y_res = y_res;

		values = (float*)malloc(x_res * y_res * sizeof(float));
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
			values[idx(i, j)] = std::max(0.0f, values[idx(i, j)] * diffuse);
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

	inline void add_heat(int x, int y, int r, float heat)
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
				add_value(x2, y2, (cosf(radius * M_PI) + 1.0f) * heat);
			}
		}
	}
};

struct game: boiler
{	
	int x_res = 200;
	int y_res = 200;

	int scale = 4;

	heat_equation simulation = heat_equation(x_res, y_res);

	void steam() override
	{
		width = x_res * scale;
		
		height = y_res * scale;

		title = "Heat equation (using Boiler)";
	}

	void draw() override
	{
		black();

		if (mouse_l)
		{
			int x = mouse_x / scale;
			int y = mouse_y / scale;

			simulation.add_heat(x, y, 20, 30.0f);
		}
		else if (mouse_r)
		{
			int x = mouse_x / scale;
			int y = mouse_y / scale;

			simulation.add_heat(x, y, 20, -3.0f);
		}

		simulation.step(0.5f, 1.0f);

		for (int j = 0; j < y_res; j++)
		for (int i = 0; i < x_res; i++)
		{
			int value = mclamprgb(simulation.values[simulation.idx(i, j)]);

			frectrgb(i * scale, j * scale, scale, scale, jet_colormap[value]);
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