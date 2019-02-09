/*

Seperating axis theorem, thanks to javidx9

	https://www.youtube.com/watch?v=7Ik2vowGcU0

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	float x;
	float y;

	point
	(
		float x,
		float y
	)
	{
		this->x = x;
		this->y = y;
	}

	point()
	{
		x = 0.0f;
		y = 0.0f;
	}
};

typedef std::vector<point> shape;

// Check if two shapes are overlapping using the separating axis theorem. The
// separating axis theorem states that two convex objects do not overlap if 
// there exists a line (called an axis) onto which the two objects'
// projections do not overlap.
//
// Description from Wikipedia.
//
// This function was kindly written by David Barr (javidx9), and was copied
// almost letter by letter from his example program.
//
// Thanks, javidx9!

bool separating_axis_theorem(shape& s1, shape& s2)
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

			float axis_x = 0.0f - (shape_1[b].y - shape_1[a].y);
			float axis_y = 0.0f + (shape_1[b].x - shape_1[a].x);

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
				float q = (shape_1[p].x * axis_x + shape_1[p].y * axis_y);

				min_1 = std::min(min_1, q);
				max_1 = std::max(max_1, q);
			}

			float min_2 = 0.0f + INFINITY;
			float max_2 = 0.0f - INFINITY;

			for (int p = 0; p < shape_2.size(); p++)
			{
				float q = (shape_2[p].x * axis_x + shape_2[p].y * axis_y);

				min_2 = std::min(min_2, q);
				max_2 = std::max(max_2, q);
			}

			overlap = std::min(std::min(max_1, max_2) - std::max(min_1, min_2), overlap);

			if (!(max_2 >= min_1 && max_1 >= min_2))
			{
				return false;
			}
		}
	}

	float s1_cx = 0.0f;
	float s1_cy = 0.0f;

	for (int i = 0; i < s1.size(); i++)
	{
		s1_cx += s1[i].x / float(s1.size());
		s1_cy += s1[i].y / float(s1.size());
	}

	float s2_cx = 0.0f;
	float s2_cy = 0.0f;

	for (int i = 0; i < s2.size(); i++)
	{
		s2_cx += s2[i].x / float(s2.size());
		s2_cy += s2[i].y / float(s2.size());
	}

	float dx = s2_cx - s1_cx;
	float dy = s2_cy - s1_cy;

	float s = sqrtf
	(
		dx * dx +
		dy * dy
	);

	for (int i = 0; i < s1.size(); i++)
	{
		s1[i].x -= overlap * dx / s * 0.5f;
		s1[i].y -= overlap * dy / s * 0.5f;
	}

	for (int i = 0; i < s2.size(); i++)
	{
		s2[i].x += overlap * dx / s * 0.5f;
		s2[i].y += overlap * dy / s * 0.5f;
	}

	return false;
}
