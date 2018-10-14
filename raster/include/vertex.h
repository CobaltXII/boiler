#pragma once

#include "vector.h"

// A vertex is a point in three dimensional space that has additional vertex data associated with
// it.

struct vertex
{
	double x;
	double y;
	double z;

	void* d = nullptr;

	vertex()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vertex(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	vertex(double _x, double _y, double _z, void* _d)
	{
		x = _x;
		y = _y;
		z = _z;

		d = _d;
	}
};

vertex vert(vector v)
{
	return vertex(v.x, v.y, v.z);
}

vector vec(vertex v)
{
	return vector(v.x, v.y, v.z);
}