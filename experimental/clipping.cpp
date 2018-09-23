// Clipping.

#include "../boiler/boiler.h"

struct game: boiler
{	
	double ax;
	double ay;

	double bx;
	double by;

	double* snx = nullptr;
	double* sny = nullptr;

	// This function will return a 4-bit number that represents which section of the screen the
	// point resides in. Using binary operations is extremely fast.

	inline char find_region(double x, double y, double tlx, double tly, double brx, double bry)
	{
		char code = 0;

		if (y <= tly)
		{
			code |= 8;
		}
		else if (y >= bry)
		{
			code |= 4;
		}

		if (x <= tlx)
		{
			code |= 1;
		}
		else if (x >= brx)
		{
			code |= 2;
		}

		return code;
	}

	// This function will clip a line so that it fits inside the specified bounds.

	bool clip_line
	(
		double x1, double y1, 
		double x2, double y2, 

		double& cx1, double& cy1, 
		double& cx2, double& cy2,

		double tlx, double tly,
		double brx, double bry
	)
	{
		char c1 = find_region(x1, y1, tlx, tly, brx, bry);
		char c2 = find_region(x2, y2, tlx, tly, brx, bry);

		bool accept;

		while (true)
		{
			if (!(c1 | c2))
			{
				// Trivial accept.

				accept = true;

				break;
			}
			else if (c1 & c2)
			{
				// Trivial reject.

				accept = false;

				break;
			}
			else
			{
				// Need to clip.

				double x;
				double y;

				char c3;

				if (c1)
				{
					c3 = c1;
				}
				else
				{
					c3 = c2;
				}

				if (c3 & 1)
				{
					x = x1 + (x2 - x1) * (bry - y1) / (y2 - y1);

					y = bry - 1.0;
				}
				else if (c3 & 2)
				{
					x = x1 + (x2 - x1) * -y1 / (y2 - y1);

					y = 0.0;
				}
				else if (c3 & 4)
				{
					y = y1 + (y2 - y1) * (brx - x1) / (x2 - x1);

					x = brx - 1.0;
				}
				else
				{
					y = y1 + (y2 - y1) * -x1 / (x2 - x1);

					x = 0.0;
				}

				if (c3 == c1)
				{
					x1 = x;
					y1 = y;

					c1 = find_region(x1, y1);
				}
				else
				{
					x2 = x;
					y2 = y;

					c2 = find_region(x2, y2);
				}
			}
		}

		if (accept)
		{
			cx1 = x1;
			cy1 = y1;

			cx2 = x2;
			cy2 = y2;

			return true;
		}
		else
		{
			cx1 = 0.0;
			cy1 = 0.0;

			cx2 = 0.0;
			cy2 = 0.0;
		}

		return false;
	}

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Cohen Sutherland clipping (using Boiler)";

		// Initialize lines.

		ax = 60.0;
		ay = 60.0;

		bx = width - 60.0;
		by = height - 60.0;
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Allow nodes to be dragged.

		if (mouse_l && snx == nullptr) 
		{
			if 
			(
				(mouse_x - ax) * (mouse_x - ax) +
				(mouse_y - ay) * (mouse_y - ay)

				<=

				(5.0 * 5.0)
			)
			{
				snx = &ax;
				sny = &ay;
			}
			else if 
			(
				(mouse_x - bx) * (mouse_x - bx) +
				(mouse_y - by) * (mouse_y - by)

				<=

				(5.0 * 5.0)
			)
			{
				snx = &bx;
				sny = &by;
			}
		}
		else if (mouse_l)
		{
			*snx = mouse_x;
			*sny = mouse_y;
		}
		else
		{
			snx = nullptr;
			sny = nullptr;
		}

		// Draw boundary.

		rectrgb
		(
			40.0,
			40.0,

			width - 80.0,
			height - 80.0,

			rgb(255, 255, 255)
		);

		// Draw unclipped line.

		linergb
		(
			ax,
			ay,

			bx,
			by,

			rgb(0, 255, 0)
		);

		// Draw nodes.

		circlergb
		(
			ax,
			ay,

			5.0,

			rgb(255, 0, 0)
		);

		circlergb
		(
			bx,
			by,

			5.0,

			rgb(255, 0, 0)
		);

		double cx;
		double cy;

		double dx;
		double dy;

		clip_line
		(
			ax, ay,
			bx, by,

			cx, cy,
			dx, dy,

			40.0, 
			40.0,

			width - 40.0,
			height - 40.0
		);

		
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