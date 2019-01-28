// Smoothed particle hydrodynamics? Awesome.

#define BOIL_PRE_INCLUDE

#include <SDL.h>

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Point count.

int n = 6084;

// Boundaries.

float x_min = 0.0f;
float x_max = 1.0f;

float y_min = 0.0f;
float y_max = 1.0f;

// Time constant.

float c_time = 0.01024f;

// Radius constant.

float c_radius = 0.016f;

// Gravitational constant.

float c_gravity = -2.56f;

// Pressure constant.

float c_pressure = 768.0f;

// Force constant.

float c_force = 4e-8f;

// Bounce constant.

float c_bounce = 25.6f;

// Mass constant.

float c_mass = 0.128f;

// Position arrays.

float* a_px;
float* a_py;

// Velocity arrays.

float* a_vx;
float* a_vy;

// Force arrays.

float* a_fx;
float* a_fy;

// Density array.

float* a_density;

// Pressure array.

float* a_pressure;

// A point.

struct point
{
	float x;
	float y;
};

// The purpose of this function is unknown. Originally named 'w'.

float function_1(float x, float y)
{
	float r2 = x * x + y * y;

	float r = sqrtf(r2);

	if (0.0f <= r && r <= c_radius)
	{
		return (M_PI * 100.0f) / (64.0f * M_PI * powf(c_radius, 4.0f)) * powf(c_radius * c_radius - r2, 3.0f);
	}
	else
	{
		return 0.0f;
	}
}

// The purpose of this function is unknown. Originally named 'dw'.

point function_2(float x, float y)
{
	float r2 = x * x + y * y;

	float r = sqrtf(r2);

	if (0.0f <= r && r <= c_radius)
	{
		point p;

		p.x = -945.0f / (32.0f * M_PI * powf(c_radius, 4.0f)) * x * powf(-c_radius * c_radius + r2, 2.0f);
		p.y = -945.0f / (32.0f * M_PI * powf(c_radius, 4.0f)) * y * powf(-c_radius * c_radius + r2, 2.0f);

		return p;
	}
	else
	{
		point p;

		p.x = 0.0f;
		p.y = 0.0f;

		return p;
	}
};

// The purpose of this function is unknown. Originally named 'ddw'.

float function_3(float x, float y)
{
	float r2 = x * x + y * y;

	float r = sqrtf(r2);

	if (0.0f <= r && r <= c_radius)
	{
		return -945.0f / (16.0f * M_PI * powf(c_radius, 4.0f)) * (powf(c_radius, 4.0f) - 4.0f * c_radius * c_radius * r + 3.0f * r * r);
	}
	else
	{
		return 0.0f;
	}
}

// This function computes the distance between two points at the indices 'i' 
// and 'j'.

float dist(int i, int j)
{
	float dx = a_px[j] - a_px[i];
	float dy = a_py[j] - a_py[i];

	return sqrtf(dx * dx + dy * dy);
}

// This function returns the smallest of two values.

float min(float x, float y)
{
	if (x < y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

// This function returns the greatest of two values.

float max(float x, float y)
{
	if (x > y)
	{
		return x;
	}
	else
	{
		return y;
	}
}

// This function returns the sign of a number. The sign of a number is 0.0f if
// the number is 0.0f. Otherwise, positive numbers have a sign of 1.0f and 
// negative numbers have a sign of -1.0f.

float signum(float x)
{
	if (x == 0.0f)
	{
		return 0.0f;
	}
	else if (x < 0.0f)
	{
		return -1.0f;
	}
	else
	{
		return 1.0f;
	}
}

// Return a random floating point number between 0.0f and 1.0f.

float rand_01_float()
{
	return float(rand()) / float(RAND_MAX);
}

// The Boiler class to use for rendering.

struct game: boiler
{	
	// Initialize Boiler.

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Smoothed particle hydrodynamics (using Boiler)";

		// Allocate arrays.

		a_px = (float*)malloc(sizeof(float) * n);
		a_py = (float*)malloc(sizeof(float) * n);

		a_vx = (float*)malloc(sizeof(float) * n);
		a_vy = (float*)malloc(sizeof(float) * n);

		a_fx = (float*)malloc(sizeof(float) * n);
		a_fy = (float*)malloc(sizeof(float) * n);

		a_density = (float*)malloc(sizeof(float) * n);

		a_pressure = (float*)malloc(sizeof(float) * n);

		// Seed the random number generator.

		srand(time(NULL));

		// Create initial conditions.

		for (int i = 0; i < n; i++)
		{
			a_vx[i] = 0.0f;
			a_vy[i] = 0.0f;

			a_fx[i] = 0.0f;
			a_fy[i] = 0.0f;

			a_density[i] = 0.0f;

			a_pressure[i] = 0.0f;
		}

		// Make a grid.

		int m = sqrtf(n);

		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < m; j++)
			{
				a_px[i * m + j] = i / float(m) * 0.5f + 0.25f;
				a_py[i * m + j] = j / float(m) * 0.5f + 0.25f;
			}
		}
	}

	// Render a frame with Boiler.

	void draw() override
	{
		black();

		// Calculate density.

		#pragma omp parallel for schedule(dynamic)

		for (int i = 0; i < n; i++)
		{
			a_density[i] = 0.0f;

			for (int j = 0; j < n; j++)
			{
				if (dist(i, j) <= c_radius)
				{
					a_density[i] += c_mass * function_1(a_px[i] - a_px[j], a_py[i] - a_py[j]);
				}
			}

			a_pressure[i] = c_pressure * a_density[i];
		}

		// Solve intersections.

		#pragma omp parallel for schedule(dynamic)

		for (int i = 0; i < n; i++)
		{
			float fpx = 0.0f;
			float fpy = 0.0f;

			float fvx = 0.0f;
			float fvy = 0.0f;

			for (int j = 0; j < n; j++)
			{
				// Check for nearby points.

				if (dist(i, j) <= c_radius)
				{
					point p = function_2(a_px[i] - a_px[j], a_py[i] - a_py[j]);

					fpx -= c_mass * (a_pressure[i] + a_pressure[j]) / (2.0f * a_density[j]) * p.x;
					fpy -= c_mass * (a_pressure[i] + a_pressure[j]) / (2.0f * a_density[j]) * p.y;

					fvx -= c_force * c_mass * (a_vx[j] - a_vx[i]) / a_density[j] * function_3(a_px[i] - a_px[j], a_py[i] - a_py[j]);
					fvy -= c_force * c_mass * (a_vy[j] - a_vy[i]) / a_density[j] * function_3(a_px[i] - a_px[j], a_py[i] - a_py[j]);
				}
			}

			// Apply force.

			a_fx[i] = fpx + fvx;
			a_fy[i] = fpy + fvy;

			// Apply gravity.

			a_fy[i] += c_gravity;

			// Clamp to boundaries.

			if (a_px[i] < x_min)
			{
				a_fx[i] = -a_fx[i];
				a_vx[i] = -a_vx[i];
			}
			else if (x_max < a_px[i])
			{
				a_fx[i] = -a_fx[i];
				a_vx[i] = -a_vx[i];
			}
			else if (a_py[i] < y_min)
			{
				a_fy[i] = -a_fy[i];
				a_vy[i] = -a_vy[i];
			}

			// Integrate using Euler integration.

			a_vx[i] += a_fx[i] * c_time;
			a_vy[i] += a_fy[i] * c_time;

			a_px[i] += a_vx[i] * c_time;
			a_py[i] += a_vy[i] * c_time;
		}

		// Draw all circles.

		for (int i = 0; i < n; i++)
		{
    		int x = a_px[i] * width;
    		int y = a_py[i] * width;

    		circlergb(x, height - y, c_radius / 2.0f * width, rgb(255, 255, 255));
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