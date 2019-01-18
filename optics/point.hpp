#pragma once

// A two-dimensional point.

struct point
{
	real x;
	real y;

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}

	point(real _x, real _y)
	{
		x = _x;
		y = _y;
	}
};

// Normalize a point.

inline point normalize(point victim)
{
	real l = sqrt(victim.x * victim.x + victim.y * victim.y);

	victim.x = victim.x / l;
	victim.y = victim.y / l;

	return victim;
}

// Calculate a random normal.

inline point random_normal()
{
	real angle = real(rand()) / real(RAND_MAX) * 2.0f * M_PI;

	point out;

	out.x = cos(angle);
	out.y = sin(angle);

	return out;
}