#pragma once

// A refractive circle.

struct refractive_circle: public intersectable
{
	// Old position.

	point op;

	// Position.

	point p;

	// Radius.

	real r;

	// Old radius anchor.

	point oa;

	// Radius anchor.

	point a;

	// Recalculate after moving.

	inline void recalculate()
	{
		op = p;

		oa = a;

		r = a.x - p.x;

		a.x = p.x + r;

		a.y = p.y;
	}

	// Check for changes.

	inline bool changed()
	{
		return op.x != p.x || op.y != p.y || oa.x != a.x || oa.y != a.y;
	}

	refractive_circle(point _p, real _r)
	{
		type = intersectable_refractive_circle;

		p = _p;

		r = _r;

		a = point(p.x + r, p.y);

		op = p;

		oa = a;
	}
};