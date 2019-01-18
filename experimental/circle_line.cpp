// Emptyness? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	double x;
	double y;

	point()
	{
		x = 0.0;
		y = 0.0;
	}

	point(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
};

struct game: boiler
{

	// Thanks to the author of this site:
	//
	//	http://csharphelper.com/blog/2014/09/determine-where-a-line-intersects-a-circle-in-c/
	//
	// I modified this a bit, so that only intersections that lie on the input line are output.

	int circle_intersect
	(
		point& point1,
		point& point2,

		point& c,

		double radius,

		point& intersection1,
		point& intersection2
	)
	{
		double dx;
		double dy;

		double A;
		double B;
		double C;

		double det;

		double t;

		dx = point2.x - point1.x;
		dy = point2.y - point1.y;

		A = dx * dx + dy * dy;

		B = 2.0 * (dx * (point1.x - c.x) + dy * (point1.y - c.y));

		C = (point1.x - c.x) * (point1.x - c.x) +
			(point1.y - c.y) * (point1.y - c.y) -

			radius * radius;

		det = B * B - 4.0 * A * C;

		if ((A <= 1e-7) || (det < 0.0))
		{
			// No real solutions.

			intersection1 = point();
			intersection2 = point();

			return 0;
		}
		else if (det == 0)
		{
			// One solution.

			t = -B / (2.0 * A);

			intersection1 = point(point1.x + t * dx, point1.y + t * dy);

			intersection2 = point();

			return 1;
		}
		else
		{
			// Two, one, or zero solutions.

			int solutions = 0;

			t = (double)((-B + sqrt(det)) / (2.0 * A));

			if (t <= 1.0 && t >= 0.0)
			{
				intersection1 = point(point1.x + t * dx, point1.y + t * dy);

				solutions++;
			}

			t = (double)((-B - sqrt(det)) / (2.0 * A));

			if (t <= 1.0 && t >= 0.0)
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

	bool within_point(point o)
	{
		if 
		(
			(mouse_x - o.x) * (mouse_x - o.x) +
			(mouse_y - o.y) * (mouse_y - o.y)

			<=

			4.0 * 4.0
		)
		{
			return true;
		}

		return false;
	}

	point a1;
	point a2;

	point b;

	double r;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Line intersection (using Boiler)";

		// Seed the random number generator.

		srand((unsigned int)time(NULL));

		// Initialize points.

		a1 = point(randx(), randy());
		a2 = point(randx(), randy());

		b = point(randx(), randy());

		r = 48.0;
	}

	double* sx;
	double* sy;

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		if (mouse_l == true && sx == nullptr)
		{
			if (within_point(a1))
			{
				sx = &(a1.x);
				sy = &(a1.y);
			}
			else if (within_point(a2))
			{
				sx = &(a2.x);
				sy = &(a2.y);
			}
			else if (within_point(b))
			{
				sx = &(b.x);
				sy = &(b.y);
			}
		}
		else if (mouse_l == true)
		{
			*sx = mouse_x;
			*sy = mouse_y;
		}
		else
		{
			sx = nullptr;
			sy = nullptr;
		}

		// Draw the line.

		linergb(a1.x, a1.y, a2.x, a2.y, rgb(255, 255, 255));

		// Draw the end points of the line. 
	
		circlergb(a1.x, a1.y, 4.0, rgb(255, 0, 0));
		circlergb(a2.x, a2.y, 4.0, rgb(255, 0, 0));

		// Draw the circle.

		circlergb(b.x, b.y, r, rgb(255, 255, 255));

		// Draw the circle's drag point.

		circlergb(b.x, b.y, 4.0, rgb(255, 0, 0));

		// Draw the intersection points of the line and the circle (if any).

		point intersection1;
		point intersection2;

		int intersect = circle_intersect(a1, a2, b, r, intersection1, intersection2);

		if (intersect == 1)
		{
			circlergb(intersection1.x, intersection1.y, 4.0, rgb(0, 255, 0));
		}
		else if (intersect == 2)
		{
			circlergb(intersection1.x, intersection1.y, 4.0, rgb(0, 255, 0));
			circlergb(intersection2.x, intersection2.y, 4.0, rgb(0, 255, 0));
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