#pragma once

// A reflective line segment.

struct reflective_segment: public intersectable
{
	// Endpoints of the segment.

	point op1;
	point op2;

	point p1;
	point p2;

	// The normal vector of the segment.

	real dx;
	real dy;

	point n;

	// Calculate the normal.

	inline void calculate_normal()
	{
		dx = p2.x - p1.x;
		dy = p2.y - p1.y;

		n = normalize(point(-dy, dx));
	}

	// Recalculate after moving.

	inline void recalculate()
	{
		op1 = p1;
		op2 = p2;

		calculate_normal();
	}

	// Check for changes.

	inline bool changed()
	{
		return op1.x != p1.x || op1.y != p1.y || op2.x != p2.x || op2.y != p2.y;
	}

	reflective_segment(point _p1, point _p2)
	{
		type = intersectable_reflective_segment;

		p1 = _p1;
		p2 = _p2;

		op1 = p1;
		op2 = p2;

		calculate_normal();
	}
};