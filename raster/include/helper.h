#pragma once

#include <cmath>

#include "vector.h"
#include "vertex.h"
#include "triangle.h"

// Convert degrees to radians.

inline double degrad(double x)
{
	return 2 * M_PI * (x / 360);
}

// Clamp a value to the range 0 to 255.

inline double clamprgb(double x)
{
	return std::max(0.0, std::min(255.0, x));
}

// Cull triangles which's normals are facing away from the camera.

inline bool cull(vector norm, triangle tri)
{
	if 
	(
		norm.x * (tri.a.x - 0) +
		norm.y * (tri.a.y - 0) +
		norm.z * (tri.a.z - 0)

		< 0.0
	)
	{
		return true;
	}

	return false;
}

// The painter's algorithm is a quick hack to sort triangles from background to foreground. It
// is not very accurate.

static bool paintersalgorithm(triangle& a, triangle& b)
{
	double z1 = (a.a.z + a.b.z + a.c.z) / 3.0;
	double z2 = (b.a.z + b.b.z + b.c.z) / 3.0;

	return z1 > z2;
}