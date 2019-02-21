// Clavet fluid? Awesome.
//
// http://www.ligum.umontreal.ca/Clavet-2005-PVFS/pvfs.pdf

#define BOIL_PRE_INCLUDE

#include <SDL.h>

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

#include "jet.h"

struct atom
{
	float x;
	float y;

	float x_prev;
	float y_prev;

	float vx;
	float vy;

	atom(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	atom(float x, float y, float vx, float vy)
	{
		this->x = x;
		this->y = y;

		this->vx = vx;
		this->vy = vy;
	}
};

struct game: boiler
{
	// The dimensions of the simulation.

	float x_res = 1024.0f;
	float y_res = 1024.0f;

	// The radius of an atom.

	float h = 15.0f;

	// The stiffness of an atom.

	float k = 5.0f;

	// The near stiffness of an atom.

	float k_near = 5.0f;

	// The rest density of an atom.

	float p0 = 5.0f;

	// The gravitational force.

	float g = 0.1f;

	// The timestep.

	float dt = 0.5f;

	// The atoms in the simulation.

	std::vector<atom> atoms;

	void steam() override
	{
		width = 960.0f;
		
		height = 960.0f;

		title = "Clavet fluid (using Boiler)";
	}

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else if (key == SDLK_s)
		{
			atoms.push_back(atom(10.0f, 10.0f, 20.0f, 5.0f));
		}
		else if (key == SDLK_d)
		{
			float dam = 50.0f;

			float brownian = 1.0f;

			float step = h;

			for (float x = -dam; x <= dam; x += step)
			for (float y = -dam; y <= dam; y += step)
			{
				atoms.push_back(atom(x + x_res / 2.0f + rand_11() * brownian, y + y_res / 2.0f + rand_11() * brownian));
			}
		}
	}

	void draw() override
	{
		black();

		// Apply gravity.

		for (int i = 0; i < atoms.size(); i++)
		{
			atoms[i].vy += dt * g;
		}

		// Save previous position and advance to predicted position.

		for (int i = 0; i < atoms.size(); i++)
		{
			atoms[i].x_prev = atoms[i].x;
			atoms[i].y_prev = atoms[i].y;

			atoms[i].x += atoms[i].vx * dt;
			atoms[i].y += atoms[i].vy * dt;
		}

		// Compute density relaxation.

		#pragma omp parallel for schedule(dynamic)

		for (int i = 0; i < atoms.size(); i++)
		{
			// Density and near density.

			float p = 0.0f;

			float p_near = 0.0f;

			// Calculate density and near density.

			#pragma omp parallel for schedule(dynamic)

			for (int j = 0; j < atoms.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				// Find the distance between particle i and particle j.

				float dx = atoms[i].x - atoms[j].x;
				float dy = atoms[i].y - atoms[j].y;

				float q = sqrtf(dx * dx + dy * dy) / h;

				if (q < 1.0f)
				{
					// Particle i and particle j are overlapping.

					float r = 1.0f - q;

					p += r * r;

					p_near += r * r * r;
				}
			}

			// Pressure and near pressure.

			float P = k * (p - p0);

			float P_near = k_near * p_near;

			// Displacement.

			float d_x = 0.0f;
			float d_y = 0.0f;

			// Calculate displacement.

			#pragma omp parallel for schedule(dynamic)

			for (int j = 0; j < atoms.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				// Find the distance between particle i and particle j.

				float dx = atoms[i].x - atoms[j].x;
				float dy = atoms[i].y - atoms[j].y;

				float d = sqrtf(dx * dx + dy * dy);

				float r_ij_x = dx / d;
				float r_ij_y = dy / d;

				float q = d / h;

				if (q < 1.0f)
				{
					// Particle i and particle j are overlapping.

					float Dx = dt * dt * (P * (1.0f - q) + P_near * (1.0f - q) * (1.0f - q)) * r_ij_x;
					float Dy = dt * dt * (P * (1.0f - q) + P_near * (1.0f - q) * (1.0f - q)) * r_ij_y;

					atoms[j].x += Dx / 2.0f;
					atoms[j].y += Dy / 2.0f;

					d_x -= Dx / 2.0f;
					d_y -= Dy / 2.0f;
				}
			}

			// Apply displacement.

			atoms[i].x += d_x;
			atoms[i].y += d_y;
		}

		// Use previous position to compute next velocity.

		for (int i = 0; i < atoms.size(); i++)
		{
			atoms[i].vx = (atoms[i].x - atoms[i].x_prev) / dt;
			atoms[i].vy = (atoms[i].y - atoms[i].y_prev) / dt;
		}

		// Boundary conditions.

		for (int i = 0; i < atoms.size(); i++)
		{
			float bounce = 0.0f;

			float& x = atoms[i].x;
			float& y = atoms[i].y;

			if (x < 0.0f)
			{
				x = 0.0f;

				atoms[i].vx = -atoms[i].vx * bounce;
			}
			else if (x > x_res)
			{
				x = x_res;

				atoms[i].vx = -atoms[i].vx * bounce;
			}

			if (y < 0.0f)
			{
				y = 0.0f;

				atoms[i].vy = -atoms[i].vy * bounce;
			}
			else if (y > y_res)
			{
				y = y_res;

				atoms[i].vy = -atoms[i].vy * bounce;
			}
		}

		// Suck up nearby particles.

		if (mouse_l)
		{
			float r = 128.0f;

			for (int i = 0; i < atoms.size(); i++)
			{
				float dx = atoms[i].x - mouse_x;
				float dy = atoms[i].y - mouse_y;

				if (dx * dx + dy * dy < r * r)
				{
					float d = sqrtf(dx * dx + dy * dy);

					float overlap = (r - d) / 64.0f;

					dx = dx / d;
					dy = dy / d;

					atoms[i].vx += dx * overlap;
					atoms[i].vy += dy * overlap;
				}
			}
		}

		// Draw all atoms.

		for (int i = 0; i < atoms.size(); i++)
		{
			float x = atoms[i].x / x_res;
			float y = atoms[i].y / y_res;

			fcirclergb(x * width, y * height, h / x_res * width, jet_colormap[63]);
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