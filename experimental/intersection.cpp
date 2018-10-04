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
	bool line_intersect
	(
		point& _a1,
		point& _a2,

		point& _b1,
		point& _b2,

		point& _o1
	)
	{
		double s1_x;
		double s1_y;

		double s2_x;
		double s2_y;

		s1_x = _a2.x - _a1.x;
		s1_y = _a2.y - _a1.y;

		s2_x = _b2.x - _b1.x;
		s2_y = _b2.y - _b1.y;

		double n1;
		double n2;

		n1 = (-s1_y * (_a1.x - _b1.x) + s1_x * (_a1.y - _b1.y)) / (-s2_x * s1_y + s1_x * s2_y);

		// I have no idea whatsoever of what the following lines of code do. However, they work, 
		// and for me that is what matters. Perhaps I will understand them someday.

		n2 = (s2_x * (_a1.y - _b1.y) - s2_y * (_a1.x - _b1.x)) / (-s2_x * s1_y + s1_x * s2_y);

		if (n1 >= 0.0 && n1 <= 1.0 && n2 >= 0.0 && n2 <= 1.0)
		{
			_o1.x = _a1.x + (n2 * s1_x);
			_o1.y = _a1.y + (n2 * s1_y);
		
			return true;
		}

		return false;
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

	point b1;
	point b2;

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

		b1 = point(randx(), randy());
		b2 = point(randx(), randy());
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
			else if (within_point(b1))
			{
				sx = &(b1.x);
				sy = &(b1.y);
			}
			else if (within_point(b2))
			{
				sx = &(b2.x);
				sy = &(b2.y);
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

		// Draw the lines.

		linergb(a1.x, a1.y, a2.x, a2.y, rgb(255, 255, 255));
		linergb(b1.x, b1.y, b2.x, b2.y, rgb(255, 255, 255));

		// Draw the end points of the lines. 
	
		circlergb(a1.x, a1.y, 4.0, rgb(255, 0, 0));
		circlergb(a2.x, a2.y, 4.0, rgb(255, 0, 0));

		circlergb(b1.x, b1.y, 4.0, rgb(0, 0, 255));
		circlergb(b2.x, b2.y, 4.0, rgb(0, 0, 255));

		// Draw the intersection point of the lines (if any).

		point i_p;

		if (line_intersect(a1, a2, b1, b2, i_p))
		{
			circlergb(i_p.x, i_p.y, 4.0, rgb(255, 0, 255));
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