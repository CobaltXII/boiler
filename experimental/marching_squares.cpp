// Marching squares? Awesome.
//
// http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

const int N = 0;
const int E = 1;
const int S = 2;
const int W = 3;

const int X = 4;

struct line_entry
{
	int num_entries;

	int entry0;
	int entry1;
	int entry2;
	int entry3;
};

line_entry line_entry_map[16] =
{
	{0, X, X, X, X},
	{2, W, S, X, X},
	{2, E, S, X, X},
	{2, W, E, X, X},
	{2, N, E, X, X},
	{4, N, W, S, E},
	{2, N, S, X, X},
	{2, N, W, X, X},

	{2, N, W, X, X},
	{2, N, S, X, X},
	{4, N, W, S, E},
	{2, N, E, X, X},
	{2, W, E, X, X},
	{2, E, S, X, X},
	{2, W, S, X, X},
	{0, X, X, X, X}
};

inline float lerp(float x0, float x1, float y0, float y1, float x)
{
	return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

struct game: boiler
{	
	int x_res = 800;
	int y_res = 800;

	int cell = 10;

	struct ball
	{
		float x;
		float y;

		float vx;
		float vy;

		float r;
	};

	std::vector<ball> balls;

	inline float field(float i, float j)
	{
		float sum = 0.0f;

		float x = cell * i;
		float y = cell * j;

		for (int i = 0; i < balls.size(); i++)
		{
			float dx = balls[i].x - x;
			float dy = balls[i].y - y;

			sum += balls[i].r * balls[i].r / (dx * dx + dy * dy);
		}

		return sum;
	}

	inline bool threshold(float x)
	{
		return x >= 1.0f;
	}

	void steam() override
	{
		width = x_res;
		
		height = y_res;

		title = "Marching squares (using Boiler)";

		for (int i = 0; i < 128; i++)
		{
			float speed = 2.0f;

			balls.push_back({rand_01f() * width, rand_01f() * height, rand_11f() * speed, rand_11f() * speed, 16.0f});
		}
	}

	void draw() override
	{
		black();

		for (int i = 0; i < balls.size(); i++)
		{
			balls[i].x += balls[i].vx;
			balls[i].y += balls[i].vy;

			if (balls[i].x < 0.0f + balls[i].r)
			{
				balls[i].x = 0.0f + balls[i].r;

				balls[i].vx = -balls[i].vx;
			}
			else if (balls[i].x >= width - balls[i].r)
			{
				balls[i].x = width - balls[i].r;

				balls[i].vx = -balls[i].vx;
			}

			if (balls[i].y < 0.0f + balls[i].r)
			{
				balls[i].y = 0.0f + balls[i].r;

				balls[i].vy = -balls[i].vy;
			}
			else if (balls[i].y >= height - balls[i].r)
			{
				balls[i].y = height - balls[i].r;

				balls[i].vy = -balls[i].vy;
			}
		}

		for (int j = 0; j < y_res / cell; j++)
		for (int i = 0; i < x_res / cell; i++)
		{
			int i0 = i;
			int j0 = j;

			int i1 = i + 1;
			int j1 = j + 1;

			float f_nw = field(i0, j0);
			float f_ne = field(i1, j0);
			float f_sw = field(i0, j1);
			float f_se = field(i1, j1);

			bool b_nw = threshold(f_nw);
			bool b_ne = threshold(f_ne);
			bool b_sw = threshold(f_sw);
			bool b_se = threshold(f_se);

			int config =
			(
				(b_sw << 0) |
				(b_se << 1) |
				(b_ne << 2) |
				(b_nw << 3)
			);

			line_entry entry = line_entry_map[config];

			float f_n = (config & 4) == (config & 8) ? 0.5f : lerp(f_nw, f_ne, 0.0f, 1.0f, 1.0f);
			float f_e = (config & 2) == (config & 4) ? 0.5f : lerp(f_ne, f_se, 0.0f, 1.0f, 1.0f);
			float f_s = (config & 1) == (config & 2) ? 0.5f : lerp(f_sw, f_se, 0.0f, 1.0f, 1.0f);
			float f_w = (config & 1) == (config & 8) ? 0.5f : lerp(f_nw, f_sw, 0.0f, 1.0f, 1.0f);

			float f_0 = 0.0f;
			float f_1 = 1.0f;

			float x_coords_lerp[4] =
			{
				i + f_n,
				i + f_1,
				i + f_s,
				i + f_0
			};

			float y_coords_lerp[4] =
			{
				j + f_0,
				j + f_e,
				j + f_1,
				j + f_w
			};

			// float x_coords[4] =
			// {
			// 	i + 0.5f,
			// 	i + 1.0f,
			// 	i + 0.5f,
			// 	i + 0.0f
			// };

			// float y_coords[4] =
			// {
			// 	j + 0.0f,
			// 	j + 0.5f,
			// 	j + 1.0f,
			// 	j + 0.5f
			// };

			if (entry.num_entries == 2)
			{
				int x0 = x_coords_lerp[entry.entry0] * cell;
				int y0 = y_coords_lerp[entry.entry0] * cell;

				int x1 = x_coords_lerp[entry.entry1] * cell;
				int y1 = y_coords_lerp[entry.entry1] * cell;

				linergb(x0, y0, x1, y1, rgb(255, 255, 255));

				// int _x0 = x_coords[entry.entry0] * cell;
				// int _y0 = y_coords[entry.entry0] * cell;

				// int _x1 = x_coords[entry.entry1] * cell;
				// int _y1 = y_coords[entry.entry1] * cell;

				// linergb(_x0, _y0, _x1, _y1, rgb(255, 0, 0));
			}
			else if (entry.num_entries == 4)
			{
				int x0 = x_coords_lerp[entry.entry0] * cell;
				int y0 = y_coords_lerp[entry.entry0] * cell;

				int x1 = x_coords_lerp[entry.entry1] * cell;
				int y1 = y_coords_lerp[entry.entry1] * cell;

				linergb(x0, y0, x1, y1, rgb(255, 255, 255));

				int x2 = x_coords_lerp[entry.entry2] * cell;
				int y2 = y_coords_lerp[entry.entry2] * cell;

				int x3 = x_coords_lerp[entry.entry3] * cell;
				int y3 = y_coords_lerp[entry.entry3] * cell;

				linergb(x2, y2, x3, y3, rgb(255, 255, 255));

				// int _x0 = x_coords[entry.entry0] * cell;
				// int _y0 = y_coords[entry.entry0] * cell;

				// int _x1 = x_coords[entry.entry1] * cell;
				// int _y1 = y_coords[entry.entry1] * cell;

				// linergb(_x0, _y0, _x1, _y1, rgb(255, 0, 0));

				// int _x2 = x_coords[entry.entry2] * cell;
				// int _y2 = y_coords[entry.entry2] * cell;

				// int _x3 = x_coords[entry.entry3] * cell;
				// int _y3 = y_coords[entry.entry3] * cell;

				// linergb(_x2, _y2, _x3, _y3, rgb(255, 0, 0));
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