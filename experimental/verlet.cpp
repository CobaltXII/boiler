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
};

// Rigid constraint.

struct constraint
{
	// Connected points.

	point* p1;
	point* p2;

	// Distance to maintain.

	real d;

	// Inactive constraints are invisible and have no effect on their 
	// contained points.

	bool active;

	// Default constructor.

	constraint
	(
		point* _p1,
		point* _p2,

		bool _active = true
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

		active = _active;
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
};
