// Affine transformations? Awesome.

#define BOIL_USE_STB_IMAGE 

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct mat3
{
	double m[3][3];
};

mat3 mat_identity()
{
	mat3 m;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			m.m[x][y] = 0.0;
		}
	}

	m.m[0][0] = 1.0;
	m.m[1][1] = 1.0;
	m.m[2][2] = 1.0;

	return m;
}

mat3 mat_translate(double tx, double ty)
{
	mat3 m;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			m.m[x][y] = 0.0;
		}
	}

	m.m[0][0] = 1.0;
	m.m[1][1] = 1.0;
	m.m[2][2] = 1.0;

	m.m[2][0] = tx;
	m.m[2][1] = ty;

	return m;
}

mat3 mat_rotate(double theta)
{
	mat3 m;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			m.m[x][y] = 0.0;
		}
	}

	m.m[0][0] = 0.0 + cos(theta);
	m.m[0][1] = 0.0 - sin(theta);
	m.m[1][0] = 0.0 + sin(theta);
	m.m[1][1] = 0.0 + cos(theta);

	m.m[2][2] = 1.0;

	return m;
}

mat3 mat_scalar(double sx, double sy)
{
	mat3 m;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			m.m[x][y] = 0.0;
		}
	}

	m.m[0][0] = sx;
	m.m[1][1] = sy;

	m.m[2][2] = 1.0;

	return m;
}
