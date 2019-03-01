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

#include "thermal_colormap.h"

#include <vector>
#include <utility>
#include <iostream>

#include "navier_stokes.h"

struct game: boiler
{
	int x_res = 256;
	int y_res = 256;

	int scale = 3;

	fluid simulation = fluid(x_res, y_res, 1e-2f, 1e-5f, 1e-24f);

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

		// Add some fluid.

		float v_factor = 1e-1f;
		
		if (mouse_l || mouse_r)
		{
			// Calculate the delta mouse position.

			int delta_x = mouse_x - mouse_ox;
			int delta_y = mouse_y - mouse_oy;

			int r = 16;

			for (int i = -r; i <= r; i++)
			for (int j = -r; j <= r; j++)
			{
				float x = sqrtf(i * i + j * j) / float(r);

				if (x < 1.0f)
				{
					float y = cosf(x * M_PI) + 1.0f;

					if (mouse_l)
					{
						simulation.add_density
						(
							mouse_x / scale + i,
							mouse_y / scale + j,

							y * 16.0
						);
					}

					y = y * y * y * y;

					simulation.add_velocity
					(
						mouse_x / scale + i, 
						mouse_y / scale + j,

						y * delta_x * v_factor,
						y * delta_y * v_factor
					);
				}
			}
		}

		// Update the fluid.

		simulation.step(1);

		// Do vorticity confinement.

		bool vorticity = true;

		if (vorticity)
		{
			simulation.vorticity_confinement(10.0f);
		}

		// Render the fluid.

		for (int j = 1; j < simulation.y_res - 1; j++)
		for (int i = 1; i < simulation.x_res - 1; i++)
		{
			frectrgb
			(
				(i - 1) * scale, 
				(j - 1) * scale, 

				scale, 
				scale, 

				sample_thermal(clamprgb(simulation.density[simulation.idx(i, j)]))
			);
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