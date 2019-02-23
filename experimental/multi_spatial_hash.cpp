// Spatial hash grid? Awesome.
//
// Note that this implementation assumes that two balls will never be within
// the same cell. This assumption is made to add a lot of speed. If the
// assumption is not true, it would be better to use a spatial hash that uses
// a list of balls within a cell.

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
};

#define MAX_ENTRIES 16

struct grid_cell
{
	size_t num_entries;

	ball* entries[MAX_ENTRIES];
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

	grid_cell* hash_grid;

	// The spatial hash neighbor table.

	int nbr_x[9] = {0 - 1, 0 + 0, 0 + 1, 0 - 1, 0 + 0, 0 + 1, 0 - 1, 0 + 0, 0 + 1};
	int nbr_y[9] = {0 - 1, 0 - 1, 0 - 1, 0 + 0, 0 + 0, 0 + 0, 0 + 1, 0 + 1, 0 + 1};

	// The radius of each ball.

	float r = 10.0f;

	void steam() override
	{
		width = 800;
		
		height = 800;

		title = "Spatial hash grid (using Boiler)";

		// Add balls.

		for (int i = 0; i < 256; i++)
		{
			// Did not know that initializer lists can be used with the
			// keyword new. This will be very helpful in the future.

			float x = width * float(rand_01());
			float y = width * float(rand_01());

			ball* new_ball = new ball
			{
				x, y,
				x, y,

				float(rand_11()) * 0.0f,
				float(rand_11()) * 0.0f
			};

			balls.push_back(new_ball);
		}

		// Initialize the spatial hash grid.

		hash_cell_xr = r * 2;
		hash_cell_yr = r * 2;

		hash_xr = width / hash_cell_xr;
		hash_yr = width / hash_cell_yr;

		hash_grid = (grid_cell*)malloc(hash_xr * hash_yr * sizeof(grid_cell));
	}

	void draw() override
	{
		black();

		// Set up the spatial hash grid.

		memset(hash_grid, 0, hash_xr * hash_yr * sizeof(grid_cell));

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
				hash_grid[h_y * hash_xr + h_x].entries[hash_grid[h_y * hash_xr + h_x].num_entries++] = b;
			}
		}

		// Draw the used spatial hash grid cells.

		for (int j = 0; j < hash_yr; j++)
		for (int i = 0; i < hash_xr; i++)
		{
			if (hash_grid[j * hash_xr + i].num_entries)
			{
				rectrgb(i * hash_cell_xr, j * hash_cell_yr, hash_cell_xr, hash_cell_yr, rgb(0, 127, 0));
			}
		}

		// Draw the balls.

		for (int i = 0; i < balls.size(); i++)
		{
			ball* b = balls[i];

			int x = b->x;
			int y = b->y;

			circlergb(x, y, r, rgb(255, 255, 255));
		}

		// Find colliding balls.

		if (false)
		{
			// Brute-force method.

			for (int i = 0; i < balls.size(); i++)
			{
				ball* b1 = balls[i];

				b1->nx = b1->x;
				b1->ny = b1->y;

				for (int j = 0; j < balls.size(); j++)
				{
					ball* b2 = balls[j];

					if (b1 == b2)
					{
						continue;
					}

					float dx = b1->x - b2->x;
					float dy = b1->y - b2->y;

					if (dx * dx + dy * dy < r * r * 4.0f)
					{
						circlergb(b1->x, b1->y, r, rgb(255, 0, 0));
						circlergb(b2->x, b2->y, r, rgb(255, 0, 0));
					}
				}
			}
		}
		else
		{
			// Spatial hash method.

			for (int i = 0; i < balls.size(); i++)
			{
				ball* b1 = balls[i];

				b1->nx = b1->x;
				b1->ny = b1->y;

				int __hx = b1->x / hash_cell_xr;
				int __hy = b1->y / hash_cell_yr;

				for (int i = 0; i < 9; i++)
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
						size_t num_entries = hash_grid[hy * hash_xr + hx].num_entries;

						for (size_t j = 0; j < num_entries; j++)
						{
							ball* b2 = hash_grid[hy * hash_xr + hx].entries[j];

							if (b1 == b2 || !b2)
							{
								continue;
							}

							float dx = b1->x - b2->x;
							float dy = b1->y - b2->y;

							if (dx * dx + dy * dy < r * r * 4.0f)
							{
								circlergb(b1->x, b1->y, r, rgb(255, 0, 0));
								circlergb(b2->x, b2->y, r, rgb(255, 0, 0));
							}
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

			b->x += b->vx;
			b->y += b->vy;
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