#pragma once

// A vector is a point in three dimensional space.

struct vector
{
	double x;
	double y;
	double z;

	vector()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	vector(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	double length()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	vector normalize()
	{
		double l = length();

		return vector(x / l, y / l, z / l);
	}
};

inline double dot(vector a, vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}