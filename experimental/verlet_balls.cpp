// Verlet sandbox? Awesome.

#define BOIL_PRE_INCLUDE

#include <SDL.h>

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef float real;

struct point
{
	real x;
	real y;

	real ox;
	real oy;

	point(real _x, real _y)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;
	}

	point(real _x, real _y, real _ox, real _oy)
	{
		x = _x;
		y = _y;

		ox = _ox;
		oy = _oy;
	}
};

struct game: boiler
{	
	// The points in the simulation.

	std::vector<point*> points;

	// The selected point.

	point* selection = nullptr;

	// The radius of each point.

	const real r = 10.0f;

	// Initialize Boiler.

	void steam() override
	{
		width = 720;
		
		height = 720;

		title = "Verlet sandbox (using Boiler)";
	}

	// Handle a key press using Boiler.

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
		else if (key == SDLK_d)
		{
			float r = 200.0f;

			float step = 20.0f;

			float brownian = 1.0f;

			for (float x = -r; x <= r; x += step)
			for (float y = -r; y <= r; y += step)
			{
				points.push_back(new point(x + width / 2.0f + rand_11() * brownian, y + height / 2.0f + rand_11() * brownian));
			}
		}
		else
		{
			points.push_back(new point(mouse_x, mouse_y));
		}
	}

	// Draw a frame using Boiler.

	void draw() override
	{
		black();

		// Circle vs. circle collisions.

		#pragma omp parallel for schedule(dynamic)

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

				real dx = p1->x - p2->x;
				real dy = p1->y - p2->y;

				real d2 = dx * dx + dy * dy;

				if (d2 < r * r * 4.0f)
				{
					real d = sqrt(d2);

					real overlap = d - r * 2.0f;

					real nx = dx / d;
					real ny = dy / d;

					p1->x -= nx * overlap / 2.0f;
					p1->y -= ny * overlap / 2.0f;

					p2->x += nx * overlap / 2.0f;
					p2->y += ny * overlap / 2.0f;

					linergb(p1->x, p1->y, p2->x, p2->y, rgb(0, 255, 0));
				}
			}
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			real vx = (p->x - p->ox) * 0.9999f;
			real vy = (p->y - p->oy) * 0.9999f;

			p->ox = p->x;
			p->oy = p->y;

			p->x += vx;
			p->y += vy;

			p->y += 0.1f;
		}

		// Drag points with left click.

		if (mouse_l && !selection)
		{
			for (int i = 0; i < points.size(); i++)
			{
				if 
				(
					(mouse_x - points[i]->x) * (mouse_x - points[i]->x) +
					(mouse_y - points[i]->y) * (mouse_y - points[i]->y) 

					<= 

					r * r
				)
				{
					selection = points[i];
				}
			}
		}
		else if (mouse_l && selection)
		{
			selection->x = mouse_x;
			selection->y = mouse_y;
		}
		else
		{
			selection = nullptr;
		}

		// Constrain all points to the boundaries.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			real vx = (p->x - p->ox) * 0.9999f;
			real vy = (p->y - p->oy) * 0.9999f;

			if (p->x + r > width)
			{
				p->x = width - r;

				p->ox = p->x + vx * 0.5f;
			}
			else if (p->x < r)
			{
				p->x = r;

				p->ox = p->x + vx * 0.5f;
			}

			if (p->y + r > height)
			{
				p->y = height - r;

				p->oy = p->y + vy * 0.5f;
			}
			else if (p->y < r)
			{
				p->y = r;

				p->oy = p->y + vy * 0.5f;
			}
		}

		// Draw all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			circlergb
			(
				p->x,
				p->y,

				r / 2.0f,

				rgb(255, 255, 255)
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