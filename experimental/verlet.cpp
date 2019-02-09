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