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


void pt_forward(mat3& mat, double ix, double iy, double& ox, double& oy)
{
	ox = ix * mat.m[0][0] + iy * mat.m[1][0] + mat.m[2][0];
	oy = ix * mat.m[0][1] + iy * mat.m[1][1] + mat.m[2][1];
}

struct game: boiler
{	
	int img_w;
	int img_h;

	Uint32* img = loadimg("test/macaw.png", img_w, img_h);

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Affine transformations (using Boiler)";
	}

	void draw() override
	{
		black();

		double f = 256.0;

		// Translate the image a distance equivalent to half of the image's dimensions towards
		// the negative quadrant. This is done so that the origin is in the center of the image. 

		mat3 ma = mat_translate(-img_w / 2, -img_h / 2);

		// Rotate the image based on the time elapsed by the application. Slow this down by a
		// factor, because the function takes radians.

		mat3 mb = mat_rotate(iteration / f);

		// Scale the image so that it appears to be pulsing along with the rotation. This adds to
		// the effect.

		mat3 mc = mat_scalar
		(
			1.0 + sin(iteration / f) * 0.5, 
			1.0 + sin(iteration / f) * 0.5
		);

		// Translate to the center of the screen, so that all the effects and transformations can
		// be easily seen.

		mat3 md = mat_translate(h_width, h_height);

		// Combine all of the transformations to create a final transformation matrix.

		mat3 transform = mat_multiply(md, mat_multiply(mb, mat_multiply(mc, ma)));

		// Invert the final transformation matrix.

		mat3 inverse = mat_inverse(transform);

