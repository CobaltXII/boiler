// Verlet integration? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <iostream>

// Naive velocity clamp.

double clampm(double val, double max)
{
	if (val > 0.0)
	{
		return std::min(val, max);
	}
	else
	{
		return std::max(val, -max);
	}
}

// A point.

struct point
{
	double px;
	double py;

	double opx;
	double opy;

	// If the locked flag is set to true, the point will not be able to move. This should also
	// change the effects of constraints and joints.

	bool locked = false;

	// Constructor, set the old position to the current position.

	point
	(
		double _px,
		double _py
	)
	{
		px = _px;
		py = _py;

		opx = _px;
		opy = _py;
	}

	// Constructor, set the old position to the current position. Optionally set the locked flag 
	// to true. There is no old and current position constructor with an optional lock flag, 
	// because that is redundant.

	point
	(
		double _px,
		double _py,

		bool _locked
	)
	{
		px = _px;
		py = _py;

		opx = _px;
		opy = _py;

		locked = _locked;
	}

	// Constructor, set the old position and the current position.

	point
	(
		double _px,
		double _py,

		double _opx,
		double _opy
	)
	{
		px = _px;
		py = _py;

		opx = _opx;
		opy = _opy;
	}
};

// A rigid constraint.

struct constraint
{
	point* p1;
	point* p2;

	double d;

	// This flag is set to false when a constraint is stretched too long. This flag is used 
	// instead of slowly removing a constraint from a vector.

	bool active = true;

	// Constructor, define the distance manually.

	constraint
	(
		point* _p1,
		point* _p2,

		double _d
	)
	{
		p1 = _p1;
		p2 = _p2;

		d = _d;
	}

	// Constructor, calculate the distance automatically.

	constraint
	(
		point* _p1,
		point* _p2
	)
	{
		p1 = _p1;
		p2 = _p2;

		double dx = p2->px - p1->px;
		double dy = p2->py - p1->py;

		d = sqrt
		(
			dx * dx +
			dy * dy
		);
	}

	// Affector.

	void do_constraint(bool mouse_r)
	{
		double dx = p2->px - p1->px;
		double dy = p2->py - p1->py;

		double cd = sqrt
		(
			dx * dx +
			dy * dy
		);

		// Calculate half of the difference between the constraint length and the current 
		// distance. Each point will be pushed away from each other by this factor.

		double nd = (d - cd) / cd / 2.0;

		double ox = dx * nd;
		double oy = dy * nd;

		if (p1->locked == false)
		{
			if (p2->locked == false)
			{		
				p1->px -= ox;
				p1->py -= oy;	

				p2->px += ox;
				p2->py += oy;
			}
			else
			{
				p1->px -= ox * 2.0;
				p1->py -= oy * 2.0;	
			}
		}
		else
		{
			if (p2->locked == false)
			{		
				p2->px += ox * 2.0;
				p2->py += oy * 2.0;
			}
		}

		// Break when stretched too long.

		if (mouse_r && nd * 2.0 < -0.85)
		{
			active = false;
		}
	}
};

struct game: boiler
{	

	// The points in the simulation.

	std::vector<point*> points;

	// The constraints in the simulation.

	std::vector<constraint> constraints;

	// Make a random point.

	point* random_point()
	{
		return new point(randx(), randy());
	}

	double clw = 60;
	double clh = 30;

	// Add fabric.

	void fabric()
	{
		double hy = 60.0;

		double hx = width / 2.0 - (10.0 * clw / 2.0) + (10.0 / 2.0);

		for (int i = 0; i < clw; i++)
		{
			for (int j = 0; j < clh; j++)
			{
				points.push_back(new point(hx + (10.0 * i), j * 10.0 + hy));
			}

			points[i * clh]->locked = true;

			for (int j = 0; j < clh - 1; j++)
			{
				constraints.push_back(constraint(points[i * clh + j], points[i * clh + j + 1]));
			}
		}

		for (int i = 0; i < clw - 1; i++)
		{
			for (int j = 0; j < clh; j++)
			{
				constraints.push_back(constraint(points[i * clh + j], points[(i + 1) * clh + j]));
			}
		}
	}

	// Handle a keypress using the Boiler framework.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			points.clear();

			constraints.clear();

			// New fabric.

			fabric();
		}
		else if (e.key.keysym.sym == SDLK_x)
		{
			if (sl != nullptr)
			{
				*sl = !(*sl);
			}
		}
	}

	// Initialize the Boiler framework.

	void steam() override
	{
		// Seed the random number generator.

		srand((unsigned int)time(NULL));

		// Initialize the Boiler window.

		width = 800;
		height = 600;

		title = "Verlet integration (using Boiler)";

		// Add fabric to the simulation.

		fabric();
	}

	double* sx = nullptr;
	double* sy = nullptr;

	bool* sl = nullptr;

	// Render a frame using the Boiler framework.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Drag points with left or right click.

		if ((mouse_l || mouse_r) && sx == nullptr)
		{
			for (int i = 0; i < points.size(); i++)
			{
				if 
				(
					(mouse_x - points[i]->px) * (mouse_x - points[i]->px) +
					(mouse_y - points[i]->py) * (mouse_y - points[i]->py) 

					<= 

					5.0 * 5.0
				)
				{
					sx = &(points[i]->px);
					sy = &(points[i]->py);

					sl = &(points[i]->locked);
				}
			}
		}
		else if ((mouse_l || mouse_r) && sx != nullptr)
		{
			*sx = mouse_x;
			*sy = mouse_y;
		}
		else
		{
			sx = nullptr;
			sy = nullptr;

			sl = nullptr;
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			if (p->locked == false)
			{
				double vx = (p->px - p->opx) * 0.99;
				double vy = (p->py - p->opy) * 0.99;

				vx = clampm(vx, 50.0);
				vy = clampm(vy, 50.0);

				p->opx = p->px;
				p->opy = p->py;

				p->px += vx;
				p->py += vy;

				p->py += 0.2;
			}
		}

		// Constrain all points to the boundaries.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			double vx = (p->px - p->opx) * 0.99;
			double vy = (p->py - p->opy) * 0.99;

			if (p->px > width)
			{
				p->px = width;

				p->opx = p->px + vx * 0.5;
			}
			else if (p->px < 0.0)
			{
				p->px = 0.0;

				p->opx = p->px + vx * 0.5;
			}

			if (p->py > height)
			{
				p->py = height;

				p->opy = p->py + vy * 0.5;
			}
			else if (p->py < 0.0)
			{
				p->py = 0.0;

				p->opy = p->py + vy * 0.5;
			}
		}

		// Apply forces of all constraints.

		for (int i = 0; i < constraints.size(); i++)
		{
			if (constraints[i].active)
			{
				constraints[i].do_constraint(mouse_r);
			}
		}

		// Render all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			circlergb
			(
				p->px,
				p->py,

				2.0,

				rgb(255, 255, 255)
			);
		}

		// Render all constraints.

		for (int i = 0; i < constraints.size(); i++)
		{
			constraint c = constraints[i];

			if (c.active)
			{
				linergb
				(
					c.p1->px,
					c.p1->py,

					c.p2->px,
					c.p2->py,

					rgb(255, 255, 255)
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