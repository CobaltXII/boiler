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

mat3 mat_multiply(mat3 a, mat3 b)
{
	mat3 out;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			out.m[i][j] = 
			(
				a.m[0][j] * b.m[i][0] + 
				a.m[1][j] * b.m[i][1] + 
				a.m[2][j] * b.m[i][2]
			);
		}
	}

	return out;
}

mat3 mat_inverse(mat3 m)
{
	mat3 out;

	float det = 
	(
		m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) -
		m.m[1][0] * (m.m[0][1] * m.m[2][2] - m.m[2][1] * m.m[0][2]) +
		m.m[2][0] * (m.m[0][1] * m.m[1][2] - m.m[1][1] * m.m[0][2])
	);

	float idet = 1.0 / det;

	out.m[0][0] = (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) * idet;
	out.m[1][0] = (m.m[2][0] * m.m[1][2] - m.m[1][0] * m.m[2][2]) * idet;
	out.m[2][0] = (m.m[1][0] * m.m[2][1] - m.m[2][0] * m.m[1][1]) * idet;
	out.m[0][1] = (m.m[2][1] * m.m[0][2] - m.m[0][1] * m.m[2][2]) * idet;
	out.m[1][1] = (m.m[0][0] * m.m[2][2] - m.m[2][0] * m.m[0][2]) * idet;
	out.m[2][1] = (m.m[0][1] * m.m[2][0] - m.m[0][0] * m.m[2][1]) * idet;
	out.m[0][2] = (m.m[0][1] * m.m[1][2] - m.m[0][2] * m.m[1][1]) * idet;
	out.m[1][2] = (m.m[0][2] * m.m[1][0] - m.m[0][0] * m.m[1][2]) * idet;
	out.m[2][2] = (m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0]) * idet;

	return out;
}
