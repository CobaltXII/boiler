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

	// The 'vx' and 'vy' arrays are used as arrays to represent the current
	// velocity of each fluid cell.

	float* vx;
	float* vy;

	// The 'vx0' and 'vy0' arrays are used as arrays to represent the previous
	// velocity of each fluid cell.

	float* vx0;
	float* vy0;

	// Default constructor.

	fluid(int x_res, int y_res, float timestep, float diffusion, float viscosity)
	{
		// Increase the resolution by 2 in each dimension, as the borders are
		// used for bounds control.

		x_res += 2;
		y_res += 2;

		// Set the parameters.

		this->x_res = x_res;
		this->y_res = y_res;

		this->timestep = timestep;

		this->diffusion = diffusion;

		this->viscosity = viscosity;

		// Allocate the arrays.

		scratch = (float*)malloc(x_res * y_res * sizeof(float));

		density = (float*)malloc(x_res * y_res * sizeof(float));

		vx = (float*)malloc(x_res * y_res * sizeof(float));
		vy = (float*)malloc(x_res * y_res * sizeof(float));

		vx0 = (float*)malloc(x_res * y_res * sizeof(float));
		vy0 = (float*)malloc(x_res * y_res * sizeof(float));
	}

	// Mike Ash: This is short for "set bounds", and it's a way to keep fluid
	// from leaking out of your box. Not that it could really leak, since it's
	// just a simulation in memory, but not having walls really screws up the
	// simulation code. Walls are added by treating the outer layer of cells
	// as the wall. Basically, every velocity in the layer next to this outer
	// layer is mirrored. So when you have some velocity towards the wall in
	// the next-to-outer layer, the wall gets a velocity that perfectly
	// counters it.

	inline void set_bnd(int b, float* x)
	{
		for (int i = 1; i < y_res - 1; i++)
		{
			x[idx(0, i)] = b == 1 ? -x[idx(1, i)] : x[idx(1, i)];

			x[idx(x_res - 1, i)] = b == 1 ? -x[idx(x_res - 1, i)] : x[idx(x_res - 1, i)];
  		}

		for (int i = 1; i < x_res - 1; i++)
		{

			x[idx(i, 0)] = b == 2 ? -x[idx(i, 1)] : x[idx(i, 1)];

			x[idx(i, y_res - 1)] = b == 2 ? -x[idx(i, y_res - 1)] : x[idx(i, y_res - 1)];
		}

		x[idx(0, 0)] = 0.5f * (x[idx(1, 0)] + x[idx(0, 1)]);

		x[idx(0, y_res - 1)] = 0.5f * (x[idx(1, y_res - 1)] + x[idx(0, y_res - 1)]);

		x[idx(x_res - 1, 0)] = 0.5f * (x[idx(x_res - 2, 0)] + x[idx(x_res - 1, 1)]);

		x[idx(x_res - 1, y_res - 1)] = 0.5f * (x[idx(x_res - 2, y_res - 1)] + x[idx(x_res - 1, y_res - 2)]);
	}

	// Mike Ash: I honestly don't know exactly what this function does or how
	// it works. What I do know is that it's used for both diffuse and
	// project. It's solving a linear differential equation of some sort,
	// although how and what is not entirely clear to me.

	inline void lin_solve(int b, float* x, float* x0, float a, float c, int iterations)
	{
		float inv_c = 1.0f / c;

		for (int k = 0; k < iterations; k++)
		{
			for (int j = 1; j < y_res - 1; j++)
			for (int i = 1; i < x_res - 1; i++)
			{
				x[idx(i, j)] = (x0[idx(i, j)] + a * (x[idx(i + 1, j)] + x[idx(i - 1, j)] + x[idx(i, j + 1)] + x[idx(i, j - 1)])) * inv_c;
			}
		}

		set_bnd(b, x);
	}

	// Mike Ash: Put a drop of soy sauce in some water, and you'll notice that
	// it doesn't stay still, but it spreads out. This happens even if the
	// water and sauce are both perfectly still. This is called diffusion. We
	// use diffusion both in the obvious case of making the dye spread out,
	// and also in the less obvious case of making the velocities of the fluid
	// spread out.

	inline void diffuse(int b, float* x, float* x0, float diffusion, float timestep, int iterations)
	{
		float a = timestep * diffusion * (x_res - 2) * (y_res - 2);

		lin_solve(b, x, x0, a, 1 + 6 * a, iterations);
	}

	// Mike Ash: Remember when I said that we're only simulating
	// incompressible fluids? This means that the amount of fluid in each box
	// has to stay constant. That means that the amount of fluid going in has
	// to be exactly equal to the amount of fluid going out. The other
	// operations tend to screw things up so that you get some boxes with a
	// net outflow, and some with a net inflow. This operation runs through
	// all the cells and fixes them up so everything is in equilibrium.

	void project(float* vx, float* vy, float* p, float* div, int iterations)
	{
		for (int j = 1; j < y_res - 1; j++)
		for (int i = 1; i < x_res - 1; i++)
		{
			div[idx(i, j)] = -0.5f * (vx[idx(i + 1, j)] - vx[idx(i - 1, j)] + vy[idx(i, j + 1)] - vy[idx(i, j - 1)]) / ((x_res + y_res) * 0.5f);
			
			p[idx(i, j)] = 0.0f;
		}

		set_bnd(0, div);

		set_bnd(0, p);

		lin_solve(0, p, div, 1, 6, iterations);

		for (int j = 1; j < y_res - 1; j++)
		for (int i = 1; i < x_res - 1; i++)
		{
			vx[idx(i, j)] -= 0.5f * (p[idx(i + 1, j)] - p[idx(i - 1, j)]) * x_res;

			vy[idx(i, j)] -= 0.5f * (p[idx(i, j + 1)] - p[idx(i, j - 1)]) * y_res;
		}

		set_bnd(1, vx);
		set_bnd(2, vy);
	}

	// Mike Ash: Every cell has a set of velocities, and these velocities
	// make things move. This is called advection. As with diffusion,
	// advection applies both to the dye and to the velocities themselves.

	void advect(int b, float* d, float* d0, float* vx, float* vy, float timestep)
	{
		float i0, i1;
		float j0, j1;

		float dtx = timestep * (x_res - 2);
		float dty = timestep * (y_res - 2);

		float s0, s1;
		float t0, t1;

		float tmp1; 
		float tmp2;

		float x;
		float y;

		float x_resfloat = x_res;
		float y_resfloat = y_res;

		float ifloat;
		float jfloat;

		int i;
		int j;

		for (j = 1, jfloat = 1; j < y_res - 1; j++, jfloat++) 
		for (i = 1, ifloat = 1; i < x_res - 1; i++, ifloat++)
		{
			tmp1 = dtx * vx[idx(i, j)];
			tmp2 = dty * vy[idx(i, j)];

			x = ifloat - tmp1; 
			y = jfloat - tmp2;

			if (x < 0.5f)
			{
				x = 0.5f;
			}

			if (x > x_resfloat + 0.5f)
			{
				x = x_resfloat + 0.5f;
			}

			i0 = floor(x);

			i1 = i0 + 1.0f;

			if (y < 0.5f)
			{
				y = 0.5f;
			}

			if (y > y_resfloat + 0.5f)
			{
				y = y_resfloat + 0.5f;
			}

			j0 = floor(y);

			j1 = j0 + 1.0f;

			s1 = x - i0;

			s0 = 1.0f - s1;

			t1 = y - j0;

			t0 = 1.0f - t1;

			int i0i = int(i0);
			int i1i = int(i1);

			int j0i = int(j0);
			int j1i = int(j1);

			d[idx(i, j)] =
			(
				s0 * (t0 * d0[idx(i0i, j0i)] + t1 * d0[idx(i0i, j1i)]) + 
				s1 * (t0 * d0[idx(i1i, j0i)] + t1 * d0[idx(i1i, j1i)])
			);
		}

		set_bnd(b, d);
	}

	// Iterate one step of the fluid simulation.

	inline void step(int iterations)
	{
		diffuse(1, vx0, vx, viscosity, timestep, iterations);
		diffuse(2, vy0, vy, viscosity, timestep, iterations);

		project(vx0, vy0, vx, vy, iterations);

		advect(1, vx, vx0, vx0, vy0, timestep);
		advect(2, vy, vy0, vx0, vy0, timestep);

		project(vx, vy, vx0, vy0, iterations);

		diffuse(0, scratch, density, diffusion, timestep, iterations);

		advect(0, density, scratch, vx, vy, timestep);
	}

	// Add density.

	inline void add_density(int x, int y, float amount)
	{
		if (x < 0 || y < 0 || x >= x_res || y >= y_res)
		{
			return;
		}

		density[idx(x, y)] += amount;
	}

	// Add velocity.

	inline void add_velocity(int x, int y, float vel_x, float vel_y)
	{
		if (x < 0 || y < 0 || x >= x_res || y >= y_res)
		{
			return;
		}

		vx[idx(x, y)] += vel_x;
		vy[idx(x, y)] += vel_y;
	}
};
