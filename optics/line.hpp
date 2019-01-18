#pragma once

// A line.

struct line
{
	point p1;
	point p2;

	line(point _p1, point _p2)
	{
		p1 = _p1;
		p2 = _p2;
	}
};

// Check if two lines intersect.

bool line_intersect
(
	point _a1,
	point _a2,

	point _b1,
	point _b2,

	point& _o1
)
{
	real s1_x;
	real s1_y;

	real s2_x;
	real s2_y;

	s1_x = _a2.x - _a1.x;
	s1_y = _a2.y - _a1.y;

	s2_x = _b2.x - _b1.x;
	s2_y = _b2.y - _b1.y;

	real n1;
	real n2;

	n1 = (-s1_y * (_a1.x - _b1.x) + s1_x * (_a1.y - _b1.y)) / (-s2_x * s1_y + s1_x * s2_y);

	// I have no idea whatsoever of what the following lines of code do.
	// However, they work, and for me that is what matters. Perhaps I will
	// understand them someday.

	n2 = (s2_x * (_a1.y - _b1.y) - s2_y * (_a1.x - _b1.x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (n1 >= 0.0 && n1 <= 1.0 && n2 >= 0.0 && n2 <= 1.0)
	{
		_o1.x = _a1.x + (n2 * s1_x);
		_o1.y = _a1.y + (n2 * s1_y);
	
		return true;
	}

	return false;
}

// Taken from /boiler/experimental/circle_line.cpp. Does not require l-values
// anymore.

int circle_intersect
(
	point point1,
	point point2,

	point c,

	real radius,

	point& intersection1,
	point& intersection2
)
{
	real dx;
	real dy;

	real A;
	real B;
	real C;

	real det;

	real t;

	dx = point2.x - point1.x;
	dy = point2.y - point1.y;

	A = dx * dx + dy * dy;

	B = 2.0f * (dx * (point1.x - c.x) + dy * (point1.y - c.y));

	C = (point1.x - c.x) * (point1.x - c.x) +
		(point1.y - c.y) * (point1.y - c.y) -

		radius * radius;

	det = B * B - 4.0f * A * C;

	if ((A <= 1e-7) || (det < 0.0f))
	{
		// No real solutions.

		intersection1 = point();
		intersection2 = point();

		return 0;
	}
	else if (det == 0)
	{
		// One solution.

		t = -B / (2.0f * A);

		intersection1 = point(point1.x + t * dx, point1.y + t * dy);

		intersection2 = point();

		return 1;
	}
	else
	{
		// Two, one, or zero solutions.

		int solutions = 0;

		t = (real)((-B + sqrtf(det)) / (2.0f * A));

		if (t <= 1.0f && t >= 0.0f)
		{
			intersection1 = point(point1.x + t * dx, point1.y + t * dy);

			solutions++;
		}

		t = (real)((-B - sqrtf(det)) / (2.0f * A));

		if (t <= 1.0f && t >= 0.0f)
		{
			if (solutions == 0)
			{
				intersection1 = point(point1.x + t * dx, point1.y + t * dy);
			}
			else
			{
				intersection2 = point(point1.x + t * dx, point1.y + t * dy);
			}

			solutions++;
		}

		return solutions;
	}
}