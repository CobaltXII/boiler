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

// Fluid simulation.

#define RENDER_COLOR

struct fluid
{
	// Dimensions of the fluid.

	int x_res;
	int y_res;

	// 1-dimensional index from 2-dimensional coordinates.

	inline int idx(int x, int y)
	{
		x = std::max(0, std::min(x_res - 1, x));
		y = std::max(0, std::min(y_res - 1, y));

		return y * x_res + x;
	}

	// The variable 'timestep' defines how much time should pass between each
	// iteration step. A higher timestep will cause a lower precision.

	float timestep;

	// The variable 'diffusion' is the diffusion factor. A lower diffusion
	// rate will cause fluid to disappear faster. A diffusion rate of 1.0f is
	// a stable fluid. Anything higher should not be used.

	float diffusion;

	// Viscosity is the fluid's resistance to flow. A higher viscosity will
	// cause the fluid to flow slower.

	float viscosity;

	// The 'scratch' array is used as a temporary array to do calculations on.
	// It should not be modified by the user.

	float* scratch;

	// The 'density' array is used as an array to represent the density of
	// each fluid cell.

	float* density;
};
