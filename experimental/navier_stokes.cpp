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

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#include "navier_stokes.h"

struct game: boiler
{
	int scale = 3;

	fluid simulation = fluid(256, 256, 0.01f, 1e-6f, 1e-9f);

	int color = 7;

	bool vorticity = true;

	bool simulating = true;

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
		else if (key == SDLK_SPACE)
		{
			simulating = !simulating;
		}
		else if (key == SDLK_v)
		{
			vorticity = !vorticity;
		}
		else if (key == SDLK_r)
		{
			for (int i = 0; i < 16; i++)
			{
				int x = rand() % (simulation.x_res - 2) + 1;
				int y = rand() % (simulation.y_res - 2) + 1;

				simulation.add_density(x, y, 10239.0f);

				float v_factor = 64.0f;

				simulation.add_velocity
				(
					x, y, 

					rand_11() * v_factor, 
					rand_11() * v_factor
				);
			}
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
		else if (key == SDLK_1)
		{
			color = 0;
		}
		else if (key == SDLK_2)
		{
			color = 1;
		}
		else if (key == SDLK_3)
		{
			color = 2;
		}
		else if (key == SDLK_4)
		{
			color = 3;
		}
		else if (key == SDLK_5)
		{
			color = 4;
		}
		else if (key == SDLK_6)
		{
			color = 5;
		}
		else if (key == SDLK_7)
		{
			color = 6;
		}
	}

	void draw() override
	{
		black();

		// Add some fluid.

		if (mouse_l || mouse_r)
		{
			// Calculate the delta mouse position.

			int delta_x = mouse_x - mouse_ox;
			int delta_y = mouse_y - mouse_oy;

			if (mouse_l)
			{
				simulation.add_density
				(
					mouse_x / scale,
					mouse_y / scale,

					1024.0f
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

		if (simulating)
		{
			// Update the fluid.

			simulation.step(1);

			// Do vorticity confinement.

			if (vorticity)
			{
				simulation.vorticity_confinement(10.0f);
			}
		}

		// Render the fluid.

		for (int j = 1; j < simulation.y_res - 1; j++)
		for (int i = 1; i < simulation.x_res - 1; i++)
		{
			if (color != 6)
			{
				unsigned char r = clamprgb(simulation.density[simulation.idx(i, j)] * 2);

				unsigned char g = clamprgb(simulation.density[simulation.idx(i, j)] * 2 - 256);
				unsigned char b = clamprgb(simulation.density[simulation.idx(i, j)] * 4 - 768);

				unsigned int color_rgb;

				if (color == 0)
				{
					color_rgb = rgb(r, g, b);
				}
				else if (color == 1)
				{
					color_rgb = rgb(r, b, g);
				}
				else if (color == 2)
				{
					color_rgb = rgb(g, r, b);
				}
				else if (color == 3)
				{
					color_rgb = rgb(g, b, r);
				}
				else if (color == 4)
				{
					color_rgb = rgb(b, r, g);
				}
				else if (color == 5)
				{
					color_rgb = rgb(b, g, r);
				}

				frectrgb
				(
					(i - 1) * scale, 
					(j - 1) * scale, 

					scale, 
					scale, 

					color_rgb
				);
			}
			else
			{
				unsigned char grey = clamprgb(simulation.density[simulation.idx(i, j)]);

				frectrgb
				(
					(i - 1) * scale, 
					(j - 1) * scale, 

					scale, 
					scale, 

					rgb(grey, grey, grey)
				);
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