/*

Doom fire? Awesome.

Thanks to Fabien Sanglard for his wonderful article:

    http://fabiensanglard.net/doom_fire_psx/index.html

Coloring algorithm from:

	https://adasba.github.io/adasba-portfolio-site/cellular_automata/life_on_fire.html

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	float x;
	float y;

	float ox;
	float oy;

	point(float _x, float _y)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;
	}

	point(float _x, float _y, float _ox, float _oy)
	{
		x = _x;
		y = _y;

		ox = _ox;
		oy = _oy;
	}
};

typedef unsigned char cell;

struct game: boiler
{	
	// Stuff to do with the Verlet balls simulation.

	std::vector<point*> points;

	float* selection_x = nullptr;
	float* selection_y = nullptr;

	const float r = 5.0f;

	// Stuff to do with the fire simulation.

	const int block_xr = 3;
	const int block_yr = 3;

	const int x_res = 600;
	const int y_res = 600;

	const int fx_res = x_res / block_xr;
	const int fy_res = y_res / block_yr;

	cell* fire_map = nullptr;

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else
		{
			points.push_back
			(
				new point
				(
					mouse_x / block_xr, 
					mouse_y / block_yr
				)
			);
		}
	}

	void steam() override
	{
		width = x_res;
		height = y_res;

		title = "Doom fire (using Boiler)";

		// Allocate fire_map.

		fire_map = (cell*)malloc(fx_res * fy_res * sizeof(cell));

		// Clear the fire_map.

		memset(fire_map, 0, fx_res * fy_res * sizeof(cell));

		// Add balls.

		srand(time(NULL));

		for (int i = 0; i < 4; i++)
		{
			points.push_back
			(
				new point
				(
					rand_11() * 10 + fx_res / 2,
					rand_11() * 10 + fy_res / 2
				)
			);
		}
	}

	void spread_fire(int idx)
	{
		// Spread fire upwards and horizontally while losing heat.

		int deviation = std::round(rand_01() * 2.4f);

		if (fire_map[idx] - (deviation & 1) * 3 >= 0)
		{
			fire_map[(idx - deviation + 1) - fx_res] = fire_map[idx] - (deviation & 1) * 3;
		}
	}

	void draw() override
	{
		black();

		// Circle vs. circle collisions.

		for (int i = 0; i < points.size(); i++)
		{
			point* p1 = points[i];

			for (int j = 0; j < points.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				point* p2 = points[j];

				float dx = p1->x - p2->x;
				float dy = p1->y - p2->y;

				float d2 = dx * dx + dy * dy;

				if (d2 < r * r * 4.0f)
				{
					float d = sqrt(d2);

					float overlap = d - r * 2.0f;

					float nx = dx / d;
					float ny = dy / d;

					p1->x -= nx * overlap / 2.0f;
					p1->y -= ny * overlap / 2.0f;

					p2->x += nx * overlap / 2.0f;
					p2->y += ny * overlap / 2.0f;
				}
			}
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			float vx = (p->x - p->ox) * 0.99f;
			float vy = (p->y - p->oy) * 0.99f;

			p->ox = p->x;
			p->oy = p->y;

			p->x += vx;
			p->y += vy;

			p->y += 0.05f;
		}

		// Drag points with left click.

		if (mouse_l && !selection_x)
		{
			for (int i = 0; i < points.size(); i++)
			{
				if 
				(
					(mouse_x / block_xr - points[i]->x) * (mouse_x / block_xr - points[i]->x) +
					(mouse_y / block_yr - points[i]->y) * (mouse_y / block_yr - points[i]->y)

					<= 

					r * r
				)
				{
					selection_x = &(points[i]->x);
					selection_y = &(points[i]->y);
				}
			}
		}
		else if (mouse_l && selection_x)
		{
			*selection_x = mouse_x / block_xr;
			*selection_y = mouse_y / block_yr;
		}
		else
		{
			selection_x = nullptr;
			selection_y = nullptr;
		}

		// Constrain all points to the boundaries.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			float vx = (p->x - p->ox) * 0.99f;
			float vy = (p->y - p->oy) * 0.99f;

			if (p->x + r > fx_res)
			{
				p->x = fx_res - r;

				p->ox = p->x + vx * 0.5f;
			}
			else if (p->x < r)
			{
				p->x = r;

				p->ox = p->x + vx * 0.5f;
			}

			if (p->y + r > fy_res)
			{
				p->y = fy_res - r;

				p->oy = p->y + vy * 0.5f;
			}
			else if (p->y < r)
			{
				p->y = r;

				p->oy = p->y + vy * 0.5f;
			}
		}

		// Propagate the fire.

		for (int x = 0; x < fx_res; x++)
		{
			for (int y = 0; y <= fy_res; y++)
			{
				spread_fire(y * fx_res + x);
			}
		}

		// Render the fire.

		for (int y = 0; y < fy_res; y++)
		{
			for (int x = 0; x < fx_res; x++)
			{
				int r = fire_map[y * fx_res + x] * 14;

				int g = fire_map[y * fx_res + x] * 14 - 256;
				int b = fire_map[y * fx_res + x] * 28 - 768;

				unsigned int color = rgb
				(
					clamprgb(r),
					clamprgb(g),
					clamprgb(b)
				);

				frectrgb(x * block_xr, y * block_yr, block_xr, block_yr, color);
			}
		}

		// Set the balls on fire.

		for (int i = 0; i < points.size(); i++)
		{
			int x = points[i]->x;
			int y = points[i]->y;

			for (int i = -r; i <= r; i++)
			for (int j = -r; j <= r; j++)
			{
				if (i * i + j * j < r * r)
				{
					if (x + i >= 0 && x + i < fx_res)
					if (y + j >= 0 && y + j < fy_res)
					{
						fire_map[(y + j) * fx_res + (x + i)] = 36;
					}
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