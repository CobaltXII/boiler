// Quadtrees? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	double x;
	double y;

	point(double _x = 0.0, double _y = 0.0)
	{
		x = _x;
		y = _y;
	}
};

// It's a dirty quadtree who does not clean up after himself.

struct quadtree
{
	quadtree* tl;
	quadtree* tr;
	quadtree* bl;
	quadtree* br;

	point* child;

	double x;
	double y;

	double w;
	double h;

	// Create an empty quadtree.

	quadtree(double _x, double _y, double _w, double _h)
	{
		x = _x;
		y = _y;

		w = _w;
		h = _h;

		tl = nullptr;
		tr = nullptr;
		bl = nullptr;
		br = nullptr;

		child = nullptr;
	}

	// Insert a point.

	void insert(point* p)
	{
		// If the width or height is 1, then don't break up any further.

		if (w == 1 || h == 1)
		{
			return;
		}

		// Check if point is in bounds.

		if (p->x > x && p->x < x + w && p->y > y && p->y < y + h)
		{
			// In bounds.

			if (tl == nullptr || tr == nullptr || bl == nullptr || br == nullptr)
			{
				if (child == nullptr)
				{
					// Just adopt this point and treat it as a child.

					child = p;
				}
				else
				{
					// Has not been subdivided yet, so subdivide.

					double hw = w / 2.0;
					double hh = h / 2.0; 

					tl = new quadtree(x, y, hw, hh);

					tr = new quadtree(x + hw, y, hw, hh);

					bl = new quadtree(x, y + hh, hw, hh);

					br = new quadtree(x + hw, y + hh, hw, hh);

					// Send our past child into one of the four corners of virtual space.

					tl->insert(child);
					tr->insert(child);
					bl->insert(child);
					br->insert(child);

					// Insert the new point to become a child of someone else.

					tl->insert(p);
					tr->insert(p);
					bl->insert(p);
					br->insert(p);
				}
			}
			else
			{
				// Has been subdivided, so propagate to children.

				tl->insert(p);
				tr->insert(p);
				bl->insert(p);
				br->insert(p);
			}
		}
		else
		{
			// Out of bounds, do nothing.

			return;
		}
	}
