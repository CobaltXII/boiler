// Verlet sandbox? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// When jelly mode is active, shapes use spring constraints instead of rigid
// constraints. Take a look at constraint::do_constraint.

bool jelly = false;

// A point.

struct point
{
	// Current position.

	float x;
	float y;

	// Previous position.

	float ox;
	float oy;

	// Locked points cannot move.

	bool locked;

	// Anchor points have a visible anchor.

	bool anchor;

	// Default constructor.

	point
	(
		float _x,
		float _y,

		bool _locked = false,
		bool _anchor = false
	)
	{
		x = _x;
		y = _y;

		ox = x;
		oy = y;

		locked = _locked;
		anchor = _anchor;
	}

	// Constructor, specify old position.

	point
	(
		float _x,
		float _y,

		float _ox,
		float _oy,

		bool _locked = false,
		bool _anchor = false
	)
	{
		x = _x;
		y = _y;

		ox = _ox;
		oy = _oy;

		locked = _locked;
		anchor = _anchor;
	}
};

// A constraint.

struct constraint
{
	// Connected points.

	point* p1;
	point* p2;

	// Distance to maintain.

	float d;

	// Visible constraints are drawn.

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

		float dx = p2->x - p1->x;
		float dy = p2->y - p1->y;

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
		float dx = p2->x - p1->x;
		float dy = p2->y - p1->y;

		float cd = sqrtf
		(
			dx * dx +
			dy * dy
		);

		// Calculate half of the difference between the constraint distance
		// and the current distance. Each point will be pushed away from each
		// other by this factor.

		float nd = (d - cd) / cd / 2.0f;

		float ox = dx * nd;
		float oy = dy * nd;

		// Currently, the simulation is rigid. To make it jelly-like, set h
		// to a small number.

		float h;

		if (jelly)
		{
			h = 0.025f;
		}
		else
		{
			h = 1.0f;
		}

		if (p1->locked == false)
		{
			if (p2->locked == false)
			{		
				p1->x -= ox * h;
				p1->y -= oy * h;	

				p2->x += ox * h;
				p2->y += oy * h;
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

// A shape.

typedef std::vector<point*> shape;

// Boiler instance.

struct game: boiler
{	
	// Universal constants.

	const float friction = 0.999f;

	const float gravity = 0.02f;

	const float bounce = 0.5f;

	const float border = 32.0f;

	// The points in the simulation.

	std::vector<point*> points;

	// The constraints in the simulation.

	std::vector<constraint*> constraints;

	// The shapes in the simulation.

	std::vector<shape> shapes;

	// The selected point (if nothing is selected, this is nullptr).

	point* selection = nullptr;

	// Resolve the collision between two shapes (if any).
	//
	// Code taken from collisions.cpp, which in turn takes the code from
	// javidx9's wonderful tutorial.

	void separating_axis_theorem(shape& s1, shape& s2)
	{
		shape shape_1 = s1;
		shape shape_2 = s2;

		float overlap = INFINITY;

		for (int shape = 0; shape < 2; shape++)
		{
			if (shape == 1)
			{
				shape_1 = s2;
				shape_2 = s1;
			}

			for (int a = 0; a < shape_1.size(); a++)
			{
				int b = (a + 1) % shape_1.size();

				float axis_x = 0.0f - (shape_1[b]->y - shape_1[a]->y);
				float axis_y = 0.0f + (shape_1[b]->x - shape_1[a]->x);

				float d = sqrtf
				(
					axis_x * axis_x +
					axis_y * axis_y
				);

				axis_x = axis_x / d;
				axis_y = axis_y / d;

				float min_1 = 0.0f + INFINITY;
				float max_1 = 0.0f - INFINITY;

				for (int p = 0; p < shape_1.size(); p++)
				{
					float q = (shape_1[p]->x * axis_x + shape_1[p]->y * axis_y);

					min_1 = std::min(min_1, q);
					max_1 = std::max(max_1, q);
				}

				float min_2 = 0.0f + INFINITY;
				float max_2 = 0.0f - INFINITY;

				for (int p = 0; p < shape_2.size(); p++)
				{
					float q = (shape_2[p]->x * axis_x + shape_2[p]->y * axis_y);

					min_2 = std::min(min_2, q);
					max_2 = std::max(max_2, q);
				}

				overlap = std::min(std::min(max_1, max_2) - std::max(min_1, min_2), overlap);

				if (!(max_2 >= min_1 && max_1 >= min_2))
				{
					return;
				}
			}
		}

		float s1_cx = 0.0f;
		float s1_cy = 0.0f;

		for (int i = 0; i < s1.size(); i++)
		{
			s1_cx += s1[i]->x / float(s1.size());
			s1_cy += s1[i]->y / float(s1.size());
		}

		float s2_cx = 0.0f;
		float s2_cy = 0.0f;

		for (int i = 0; i < s2.size(); i++)
		{
			s2_cx += s2[i]->x / float(s2.size());
			s2_cy += s2[i]->y / float(s2.size());
		}

		float dx = s2_cx - s1_cx;
		float dy = s2_cy - s1_cy;

		float s = sqrtf
		(
			dx * dx +
			dy * dy
		);

		float h = 0.5f;

		for (int i = 0; i < s1.size(); i++)
		{
			float dx = s2_cx - s1[i]->x;
			float dy = s2_cy - s1[i]->y;

			float s = sqrtf
			(
				dx * dx +
				dy * dy
			);

			s1[i]->x -= overlap * dx / s * h;
			s1[i]->y -= overlap * dy / s * h;
		}

		for (int i = 0; i < s2.size(); i++)
		{
			float dx = s2[i]->x - s1_cx;
			float dy = s2[i]->y - s1_cy;

			float s = sqrtf
			(
				dx * dx +
				dy * dy
			);

			s2[i]->x += overlap * dx / s * h;
			s2[i]->y += overlap * dy / s * h;
		}
	}

	// Generate a random polygon.

	void random_polygon(float cx, float cy, float vx, float vy)
	{
		int sides = 3 + rand() % 5;

		float radius = (1.0f + float(rand()) / float(RAND_MAX)) * 16.0f;

		float rot = degrad(360.0f / (sides * 2));

		float x[sides];
		float y[sides];

		for (int i = 0; i < sides; i++)
		{
			x[i] = cx + cos(rot + degrad(360.0f / sides * i)) * radius;
			y[i] = cy + sin(rot + degrad(360.0f / sides * i)) * radius;
		}

		point* p[sides];

		for (int i = 0; i < sides; i++)
		{
			points.push_back(p[i] = new point(x[i], y[i], x[i] - vx, y[i] - vy));
		}


		for (int i = 0; i < sides; i++)
		{
			constraints.push_back(new constraint(p[i], p[(i + 1) % sides]));
		}

		point* center = new point(cx, cy, false, true);

		points.push_back(center);

		for (int i = 0; i < sides; i++)
		{
			constraints.push_back(new constraint(center, p[i], false));
		}

		shape new_shape;

		for (int i = 0; i < sides; i++)
		{
			new_shape.push_back(p[i]);
		}

		shapes.push_back(new_shape);

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
	}

	// Handle a key press using Boiler.

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_SPACE)
		{
			float vx = rand_11() * 0.1f;
			float vy = rand_11() * 0.1f;

			random_polygon
			(
				mouse_x - border, 
				mouse_y - border, 

				vx, vy
			);
		}
		if (key == SDLK_q)
		{
			points.clear();

			constraints.clear();

			shapes.clear();
		}
		if (key == SDLK_j)
		{
			jelly = !jelly;
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

	// Draw a frame with Boiler.

	void draw() override
	{
		black();

		// Shape vs. shape collisions.

		for (int i = 0; i < shapes.size(); i++)
		{
			for (int j = 0; j < shapes.size(); j++)
			{
				if (i == j)
				{
					continue;
				}

				separating_axis_theorem(shapes[i], shapes[j]);
			}
		}

		// Update all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			if (!p->locked)
			{
				float vx = (p->x - p->ox) * friction;
				float vy = (p->y - p->oy) * friction;

				p->ox = p->x;
				p->oy = p->y;

				p->x += vx;
				p->y += vy;

				p->y += gravity;
			}
		}

		// Constrain all points to the boundaries.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			float vx = (p->x - p->ox) * friction;
			float vy = (p->y - p->oy) * friction;

			if (p->x > width - border * 2.0f)
			{
				p->x = width - border * 2.0f;

				p->ox = p->x + vx * bounce;
			}
			else if (p->x < 0.0f)
			{
				p->x = 0.0f;

				p->ox = p->x + vx * bounce;
			}

			if (p->y > height - border * 2.0f)
			{
				p->y = height - border * 2.0f;

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
			constraints[i]->do_constraint();
		}

		// Draw all points.

		for (int i = 0; i < points.size(); i++)
		{
			point* p = points[i];

			if (p->anchor)
			{
				unsigned int color = rgb(255, 255, 255);

				if (p->locked)
				{
					color = rgb(255, 0, 0);
				}

				circlergb
				(
					p->x + border,
					p->y + border,

					3,

					color
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