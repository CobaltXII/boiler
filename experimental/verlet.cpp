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
