#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>

#include "helper.h"
#include "vector.h"

// A matrix.

typedef double** matrix;

// This function will create an empty matrix of a given size. If this is not used, then a
// segmentation fault will occur.

inline matrix mmatrix(unsigned int i, unsigned int j)
{
	double** m = (double**)malloc(i * sizeof(double*));

	for (int k = 0; k < i; k++)
	{
		m[k] = (double*)malloc(j * sizeof(double));

		memset((void*)m[k], 0, j * sizeof(double));
	}

	return m;
}

// Destroy a matrix.

inline void mdestroy(matrix m, int i)
{
	for (int k = 0; k < i; k++)
	{
		free((void*)m[k]);
	}

	free(m);
}

// Generate a 4 by 4 rotation matrix along the x axis.

inline void mrotx(matrix m, double a)
{
	memset((void*)m[0], 0, 4 * sizeof(double));
	memset((void*)m[1], 0, 4 * sizeof(double));
	memset((void*)m[2], 0, 4 * sizeof(double));
	memset((void*)m[3], 0, 4 * sizeof(double));

	m[0][0] = 1;
	m[3][3] = 1;

	m[1][1] = 0 + cos(a);
	m[1][2] = 0 + sin(a);
	m[2][1] = 0 - sin(a);
	m[2][2] = 0 + cos(a);
}

// Generate a 4 by 4 rotation matrix along the y axis.

inline void mroty(matrix m, double a)
{
	memset((void*)m[0], 0, 4 * sizeof(double));
	memset((void*)m[1], 0, 4 * sizeof(double));
	memset((void*)m[2], 0, 4 * sizeof(double));
	memset((void*)m[3], 0, 4 * sizeof(double));

	m[1][1] = 1;
	m[3][3] = 1;

	m[0][0] = 0 + cos(a);
	m[2][0] = 0 + sin(a);
	m[0][2] = 0 - sin(a);
	m[2][2] = 0 + cos(a);
}

// Generate a 4 by 4 rotation matrix along the z axis.

inline void mrotz(matrix m, double a)
{
	memset((void*)m[0], 0, 4 * sizeof(double));
	memset((void*)m[1], 0, 4 * sizeof(double));
	memset((void*)m[2], 0, 4 * sizeof(double));
	memset((void*)m[3], 0, 4 * sizeof(double));

	m[2][2] = 1;
	m[3][3] = 1;

	m[0][0] = 0 + cos(a);
	m[0][1] = 0 + sin(a);
	m[1][0] = 0 - sin(a);
	m[1][1] = 0 + cos(a);
}

// Generate a projection matrix. This part of software rendering is like black magic; I still
// don't fully understand why it works.

inline matrix mproj(double f_near, double f_far, double f_fov)
{
	matrix m = mmatrix(4, 4);

	double f_rad = 1.0f / tanf(f_fov / 360.0 * M_PI);

	m[2][3] = 1.0;
	m[3][3] = 0.0;

	m[0][0] = f_rad;
	m[1][1] = f_rad;

	// I am always considering the aspect ratio to be 1, because if the aspect ratio is any
	// different then the image appears skewed, which is not desired in my applications.

	m[2][2] = f_far / (f_far - f_near);

	// The distance between f_far and f_near is important because it displaces objects a bit
	// further from the camera than they actually are. This makes rendering look more real.

	m[3][2] = (-f_far * f_near) / (f_far - f_near);

	return m;
}

// This function will multiply vector i by matrix m, and divide vector i by the fourth
// coordinate of the matrix multiplication, and store the result vector in o.

void matvec(vector &i, vector &o, matrix &m)
{
	o.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + m[3][0];
	o.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + m[3][1];
	o.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + m[3][2];

	double w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + m[3][3];

	if (w != 0.0)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}

void matvec(vertex &i, vertex &o, matrix &m)
{
	o.x = i.x * m[0][0] + i.y * m[1][0] + i.z * m[2][0] + m[3][0];
	o.y = i.x * m[0][1] + i.y * m[1][1] + i.z * m[2][1] + m[3][1];
	o.z = i.x * m[0][2] + i.y * m[1][2] + i.z * m[2][2] + m[3][2];

	double w = i.x * m[0][3] + i.y * m[1][3] + i.z * m[2][3] + m[3][3];

	if (w != 0.0)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}