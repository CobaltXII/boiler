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
