// Diffusion limited aggregation. Using a spatial hash implementation from my
// earlier code.
//
// New code is marked with a comment reading "Used by DLA.".

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct ball
{
	float x;
	float y;

	float nx;
	float ny;

	float vx;
	float vy;

	// Used by DLA.
	bool stuck = false;

	ball(float x, float y, float nx, float ny, float vx, float vy, bool stuck) {
		this->x = x;
		this->y = y;
		this->nx = nx;
		this->ny = ny;
		this->vx = vx;
		this->vy = vy;
		this->stuck = stuck;
	}
};

bool repulsion = true;

struct game: boiler
{	
	// The balls in the simulation.

	std::vector<ball*> balls;

	// The spatial hash grid.

	int hash_cell_xr;
	int hash_cell_yr;

	int hash_xr;
	int hash_yr;

	ball** hash_grid;

	// The spatial hash neighbor table.

	int nbr_x[8] = {0 - 1, 0 + 0, 0 + 1, 0 - 1, 0 + 1, 0 - 1, 0 + 0, 0 + 1};
	int nbr_y[8] = {0 - 1, 0 - 1, 0 - 1, 0 + 0, 0 + 0, 0 + 1, 0 + 1, 0 + 1};

	// The radius of each ball.

	float r = 4.0f;

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Spatial hash grid (using Boiler)";

		// Add balls.

		for (int i = 0; i < 2048; i++)
		{
			// Did not know that initializer lists can be used with the
			// keyword new. This will be very helpful in the future.

			float x = width * float(rand_01());
			float y = width * float(rand_01());

			ball* new_ball = new ball
			(
				x, y,
				x, y,

				float(rand_11()) * 1.0f,
				float(rand_11()) * 1.0f,

				// Used by DLA.
				false
			);

			balls.push_back(new_ball);
		}

		// Used by DLA.
		// Add the initial stuck ball.
		balls.push_back(new ball(0.5f * width, 0.5f * width, 0.5f * width, 0.5f * width, 0.0f, 0.0f, true));

		// Initialize the spatial hash grid.

		hash_cell_xr = r * 2;
		hash_cell_yr = r * 2;

		hash_xr = width / hash_cell_xr;
		hash_yr = width / hash_cell_yr;

		hash_grid = (ball**)malloc(hash_xr * hash_yr * sizeof(ball*));
	}

	void draw() override
	{
		black();

		for (int loops = 0; loops < 10; loops++) {

		// Set up the spatial hash grid.

		memset(hash_grid, 0, hash_xr * hash_yr * sizeof(ball*));

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b = balls[i];

			int h_x = b->x / hash_cell_xr;
			int h_y = b->y / hash_cell_yr;

			if
			(
				h_x >= 0 && h_x < hash_xr &&
				h_y >= 0 && h_y < hash_yr
			)
			{
				hash_grid[h_y * hash_xr + h_x] = b;
			}
		}

		// Draw the balls.

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b = balls[i];

			int x = b->x;
			int y = b->y;

			// Used by DLA.
			if (b->stuck) {
				circlergb(x, y, r, rgb(255, 0, 0));
			} else {
				circlergb(x, y, r, rgb(255, 255, 255));
			}
		}

		// Find colliding balls.

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b1 = balls[i];

			b1->nx = b1->x;
			b1->ny = b1->y;

			int __hx = b1->x / hash_cell_xr;
			int __hy = b1->y / hash_cell_yr;

			for (int i = 0; i < 8; i++)
			{
				int off_x = nbr_x[i];
				int off_y = nbr_y[i];

				int hx = __hx + off_x;
				int hy = __hy + off_y;

				if
				(
					hx >= 0 && hx < hash_xr &&
					hy >= 0 && hy < hash_yr
				)
				{
					ball* b2 = hash_grid[hy * hash_xr + hx];

					if (b1 == b2 || !b2)
					{
						continue;
					}

					float dx = b1->x - b2->x;
					float dy = b1->y - b2->y;

					if (dx * dx + dy * dy < r * r * 4.0f)
					{
						// Colliding.
						if (b1->stuck || b2->stuck) {
							b1->stuck = true;
							b2->stuck = true;
						}
					}
				}
			}
		}

		// Integrate the balls.

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b = balls[i];

			b->x = b->nx;
			b->y = b->ny;

			// Used by DLA.
			if (!b->stuck) {
				b->x += float(rand_11());
				b->y += float(rand_11());
			}
		}

		// Constrain the balls to the boundaries.

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b = balls[i];

			if (b->x < 0.0f + r)
			{
				b->x = 0.0f + r;

				b->vx = -b->vx;
			}
			else if (b->x >= width - r)
			{
				b->x = width - r;

				b->vx = -b->vx;
			}

			if (b->y < 0.0f + r)
			{
				b->y = 0.0f + r;

				b->vy = -b->vy;
			}
			else if (b->y >= height - r)
			{
				b->y = height - r;

				b->vy = -b->vy;
			}
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