// Navier-Stokes fluid simulation? Awesome.
//
// Based on Daniel Shiffman's 2-dimensional fluid simulation:
//
//     https://github.com/shiffman/CFD/blob/master/CFDash/CFDash.pde
//     https://github.com/shiffman/CFD/blob/master/CFDash/Fluid.pde
//
// which is based on Mike Ash's superb tutorial:
//
//     https://mikeash.com/pyblog/fluid-simulation-for-dummies.html
//
// which in turn is based on Jos Stam's spectacular paper:
//
//     http://www.dgp.toronto.edu/people/stam/reality/Research/pdf/GDC03.pdf
//
// Marching squares implementation based on the code by Jamie Wong
//
// http://jamie-wong.com/2014/08/19/metaballs-and-marching-squares/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#include "navier_stokes.h"

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

inline float threshold(float x)
{
	return x >= 256.0f;
}

struct game: boiler
{
	int x_res = 256;
	int y_res = 256;

	int scale = 3;

	fluid simulation = fluid(x_res, y_res, 1e-2f, 1e-8f, 0.0f);

	void steam() override
	{
		width = (simulation.x_res - 2) * scale;

		height = (simulation.y_res - 2) * scale;

		title = "Navier-Stokes fluid simulation (using Boiler)";
	}

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else if (key == SDLK_c)
		{
			for (int j = 0; j < simulation.y_res; j++)
			for (int i = 0; i < simulation.x_res; i++)
			{
				simulation.vx[simulation.idx(i, j)] = 0.0f;
				simulation.vy[simulation.idx(i, j)] = 0.0f;

				simulation.density[simulation.idx(i, j)] = 0.0f;
			}
		}
	}

	void draw() override
	{
		black();

		if (mouse_l || mouse_r)
		{
			int delta_x = mouse_x - mouse_ox;
			int delta_y = mouse_y - mouse_oy;

			if (mouse_l)
			{
				simulation.add_density
				(
					mouse_x / scale,
					mouse_y / scale,

					1024.0f * 16.0f
				);
			}

			float v_factor = 2.0f;

			simulation.add_velocity
			(
				mouse_x / scale, 
				mouse_y / scale,

				delta_x * v_factor,
				delta_y * v_factor
			);
		}

		simulation.step(1);

		simulation.vorticity_confinement(10.0f);

		for (int j = 0; j < simulation.y_res; j++)
		for (int i = 0; i < simulation.x_res; i++)
		{
			int i0 = i;
			int j0 = j;

			int i1 = i + 1;
			int j1 = j + 1;

			float f_nw = simulation.density[simulation.idx(i0, j0)];
			float f_ne = simulation.density[simulation.idx(i1, j0)];
			float f_sw = simulation.density[simulation.idx(i0, j1)];
			float f_se = simulation.density[simulation.idx(i1, j1)];

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

			float f_n = (config & 4) == (config & 8) ? 0.5f : lerp(f_nw, f_ne, 0.0f, 1.0f, 256.0f);
			float f_e = (config & 2) == (config & 4) ? 0.5f : lerp(f_ne, f_se, 0.0f, 1.0f, 256.0f);
			float f_s = (config & 1) == (config & 2) ? 0.5f : lerp(f_sw, f_se, 0.0f, 1.0f, 256.0f);
			float f_w = (config & 1) == (config & 8) ? 0.5f : lerp(f_nw, f_sw, 0.0f, 1.0f, 256.0f);

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

			if (entry.num_entries == 2)
			{
				int x0 = x_coords_lerp[entry.entry0] * scale;
				int y0 = y_coords_lerp[entry.entry0] * scale;

				int x1 = x_coords_lerp[entry.entry1] * scale;
				int y1 = y_coords_lerp[entry.entry1] * scale;

				linergb(x0, y0, x1, y1, rgb(255, 255, 255));
			}
			else if (entry.num_entries == 4)
			{
				int x0 = x_coords_lerp[entry.entry0] * scale;
				int y0 = y_coords_lerp[entry.entry0] * scale;

				int x1 = x_coords_lerp[entry.entry1] * scale;
				int y1 = y_coords_lerp[entry.entry1] * scale;

				linergb(x0, y0, x1, y1, rgb(255, 255, 255));

				int x2 = x_coords_lerp[entry.entry2] * scale;
				int y2 = y_coords_lerp[entry.entry2] * scale;

				int x3 = x_coords_lerp[entry.entry3] * scale;
				int y3 = y_coords_lerp[entry.entry3] * scale;

				linergb(x2, y2, x3, y3, rgb(255, 255, 255));
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