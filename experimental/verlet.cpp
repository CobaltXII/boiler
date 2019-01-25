// Verlet sandbox? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

typedef float real;

// Naive velocity clamp.

real clampm(real val, real clamp)
{
	if (val > 0.0f)
	{
		return std::min(val, clamp);
	}
	else
	{
		return std::max(val, -clamp);
	}
}

// A point.

struct point
{
	// Current position.

	real x;
	real y;

	// Previous position.

	real ox;
	real oy;

	// Locked points cannot move.

	bool locked;

	// Default constructor.

	point
	(
		real _x,
		real _y,

		bool _locked = false
	)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;

		locked = _locked;
	}

	// Constructor, specify old position.

	point
	(
		real _x,
		real _y,

		real _ox,
		real _oy,

		bool _locked = false
	)
	{
		x = _x;
		y = _y;

		ox = _ox;
		oy = _oy;

		locked = _locked;
	}
};

// Rigid constraint.

struct constraint
{
	// Connected points.

	point* p1;
	point* p2;

	// Distance to maintain.

	real d;

	// Visible constraints are, well, visible.

	bool visible;

	// Default constructor.

	constraint
	(
		point* _p1,
		point* _p2,

		bool _visible = true
	)
	{
		p1 = _p1;
		p2 = _p2;

		real dx = p2->x - p1->x;
		real dy = p2->y - p1->y;

		d = sqrtf
		(
			dx * dx +
			dy * dy
		);

		visible = _visible;
	}

	// Do constraint.

	void do_constraint()
	{
		real dx = p2->x - p1->x;
		real dy = p2->y - p1->y;

		real cd = sqrtf
		(
			dx * dx +
			dy * dy
		);

		// Calculate half of the difference between the constraint distance
		// and the current distance. Each point will be pushed away from each
		// other by this factor.

		real nd = (d - cd) / cd / 2.0f;

		real ox = dx * nd;
		real oy = dy * nd;

		if (p1->locked == false)
		{
			if (p2->locked == false)
			{		
				p1->x -= ox;
				p1->y -= oy;	

				p2->x += ox;
				p2->y += oy;
			}
			else
			{
				p1->x -= ox * 2.0;
				p1->y -= oy * 2.0;
			}
		}
		else
		{
			if (p2->locked == false)
			{		
				p2->x += ox * 2.0;
				p2->y += oy * 2.0;
			}
		}
	}
};

// A circle.

struct circle
{
	// The position.

	point* p;

	// The radius.

	real r;

	// Default constructor.

	circle
	(
		point* _p,

		real _r
	)
	{
		p = _p;

		r = _r;
	}

	// Check if a point is within this circle.

	inline bool within(point* _p)
	{
		return
		(
			(_p->x - p->x) * (_p->x - p->x) +
			(_p->y - p->y) * (_p->y - p->y)

			<= r * r
		);
	}
};

struct game: boiler
{	
	// Universal constants.

	const real friction = 0.99f;

	const real gravity = 0.20f;

	const real bounce = 0.50f;

	// The points in the simulation.

	std::vector<point*> points;

	// The constraints in the simulation.

	std::vector<constraint*> constraints;

	// The circles in the simulation.

	std::vector<circle*> circles;

	// The selected point (if nothing is selected, nullptr).

	point* selection = nullptr;

	// Generate a random point.

	point* random_point()
	{
		return new point(randx(), randy());
	}

	// Generate a random polygon.

	void random_polygon(real cx, real cy, real vx, real vy)
	{
		int sides = rand() % 5 + 3;

		real radius = (1.0f + real(rand()) / real(RAND_MAX)) * 12.0f;

		real rot = real(rand()) / real(RAND_MAX) * 2.0f * M_PI;

		real x[sides];
		real y[sides];

		for (int i = 0; i < sides; i++)
		{
			x[i] = cx + cos(rot + degrad(360.0f / sides * i)) * radius;
			y[i] = cy + sin(rot + degrad(360.0f / sides * i)) * radius;
		}

		point* p[sides];

		for (int i = 0; i < sides; i++)
		{
			points.push_back(p[i] = new point(x[i], y[i], x[i] - vx, y[i] - vy));

			circles.push_back(new circle(p[i], 3.0f));
		}

		for (int i = 0; i < sides; i++)
		{
			constraints.push_back(new constraint(p[i], p[(i + 1) % sides]));
		}

		for (int i = 0; i < sides; i++)
		{
			for (int j = 0; j < sides; j++)
			{
				if (i == j)
				{
					continue;
				}

				constraints.push_back(new constraint(p[i], p[j], false));
			}
		}
	}

	// Initialize Boiler.

	void steam() override
	{
		width = 800;
		
		height = 600;

		title = "Verlet sandbox (using Boiler)";

		// Add a rope.

		if (false)
		{
			int segments = 48;

			real length = 256.0f;

			point* r[segments];

			for (int i = 0; i < segments; i++)
			{
				points.push_back(r[i] = new point(width / 2, 32 + (length / segments) * (i + 1)));

				circles.push_back(new circle(r[i], 3.0f));
			}

			r[0]->locked = true;

			for (int i = 0; i < segments - 1; i++)
			{
				constraints.push_back(new constraint(r[i], r[i + 1]));
			}
		}

		// Add a cloth.

		int x_segments = 32;
		int y_segments = 32;

		real x_length = 384.0f;
		real y_length = 384.0f;

		point* c[x_segments][y_segments];

		for (int i = 0; i < x_segments; i++)
		{
			real x = width / 2 - (x_length / 2.0f) + ((i + 1) * (x_length / x_segments));

			for (int j = 0; j < y_segments; j++)
			{
				real y = 32.0f + ((j + 1) * (y_length / y_segments));

				points.push_back(c[i][j] = new point(x, y));

				if (!j)
				{
					c[i][j]->locked = true;
				}
			}
		}

		for (int i = 0; i < x_segments - 1; i++)
		{
			for (int j = 0; j < y_segments; j++)
			{
				constraints.push_back(new constraint(c[i][j], c[i + 1][j]));
			}
		}

		for (int i = 0; i < x_segments; i++)
		{
			for (int j = 0; j < y_segments - 1; j++)
			{
				constraints.push_back(new constraint(c[i][j], c[i][j + 1]));
			}
		}
	}

	// Handle a key press using Boiler.

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_SPACE)
		{
			real vx = (real(rand()) / real(RAND_MAX) * 2.0f - 1.0f) * 4.0f;
			real vy = (real(rand()) / real(RAND_MAX) * 2.0f - 1.0f) * 4.0f;

			random_polygon(mouse_x, mouse_y, vx, vy);
		}
		if (key == SDLK_c)
		{
			point* p = new point
			(
				mouse_x,
				mouse_y
			);

			points.push_back(p);

			circles.push_back(new circle(p, 15.0f));
		}
		if (key == SDLK_l)
		{
			if (selection)
			{
				selection->locked = !selection->locked;
			}
		}
		else if (key == SDLK_ESCAPE)
		{
			running = SDL_FALSE;
		}
	}

	// Draw a frame using Boiler.

	void draw() override
	{
		black();

		// Circle vs. circle collisions.

		for (int i = 0; i < circles.size(); i++)
		{
			circle* c1 = circles[i];

			for (int j = 0; j < circles.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				circle* c2 = circles[j];

				if (c1->p->locked && c2->p->locked)
				{
					// No point to continue if none can be moved.

					continue;
				}

				real dx = c1->p->x - c2->p->x;
				real dy = c1->p->y - c2->p->y;

				real d2 = dx * dx + dy * dy;

				if (d2 < (c1->r + c2->r) * (c1->r + c2->r))
				{
					// Intersecting, find overlap distance.

					real d = sqrt(d2);

					real overlap = d - c1->r - c2->r;

					real nx = dx / d;
					real ny = dy / d;

					if (!c1->p->locked && !c2->p->locked)
					{
						c1->p->x -= nx * overlap / 2.0f;
						c1->p->y -= ny * overlap / 2.0f;

						c2->p->x += nx * overlap / 2.0f;
						c2->p->y += ny * overlap / 2.0f;
					}
					else if (c1->p->locked)
					{
						c2->p->x += nx * overlap;
						c2->p->y += ny * overlap;
					}
					else
					{
						c1->p->x -= nx * overlap;
						c1->p->y -= ny * overlap;
					}
				}
			}
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			if (p->locked == false)
			{
				real vx = (p->x - p->ox) * friction;
				real vy = (p->y - p->oy) * friction;

				vx = clampm(vx, 50.0f);
				vy = clampm(vy, 50.0f);

				p->ox = p->x;
				p->oy = p->y;

				p->x += vx;
				p->y += vy;

				p->y += gravity;

				for (int j = 0; j < circles.size(); j++)
				{
					// Circle collision detection and response.

					circle* c = circles[j];

					if (c->p == p)
					{
						// Can't have circles intersecting themselves!

						continue;
					}

					if (c->within(p))
					{
						real d = sqrt
						(
							(p->x - c->p->x) * (p->x - c->p->x) +
							(p->y - c->p->y) * (p->y - c->p->y)
						);

						real nx = (p->x - c->p->x) / d;
						real ny = (p->y - c->p->y) / d;

						p->x = c->p->x + nx * c->r;
						p->y = c->p->y + ny * c->r;
					}
				}
			}
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

					5.0f * 5.0f
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

			real vx = (p->x - p->ox) * 0.99f;
			real vy = (p->y - p->oy) * 0.99f;

			if (p->x > width)
			{
				p->x = width;

				p->ox = p->x + vx * bounce;
			}
			else if (p->x < 0.0f)
			{
				p->x = 0.0f;

				p->ox = p->x + vx * bounce;
			}

			if (p->y > height - 64.0f)
			{
				p->y = height - 64.0f;

				p->oy = p->y + vy * bounce;
			}
			else if (p->y < 0.0f)
			{
				p->y = 0.0f;

				p->oy = p->y + vy * bounce;
			}
		}

		// Apply forces of all constraints.

		for (int i = 0; i < constraints.size(); i++)
		{
			constraint* c = constraints[i];

			c->do_constraint();
		}

		// Apply forces of all constraints in reverse order.

		for (int i = 1; i <= constraints.size(); i++)
		{
			constraint* c = constraints[constraints.size() - i];

			c->do_constraint();
		}

		// Draw all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			unsigned int color = rgb(255, 255, 255);

			if (p->locked)
			{
				color = rgb(255, 0, 0);
			}

			circlergb
			(
				p->x,
				p->y,

				3,

				color
			);
		}

		// Draw all constraints.

		for (int i = 0; i < constraints.size(); i++)
		{
			constraint* c = constraints[i];

			if (c->visible)
			{
				linergb
				(
					c->p1->x,
					c->p1->y,

					c->p2->x,
					c->p2->y,

					rgb(255, 255, 255)
				);
			}
		}

		// Draw all circles.

		for (int i = 0; i < circles.size(); i++)
		{
			circle* c = circles[i];

			circlergb
			(
				c->p->x,
				c->p->y,

				c->r,

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