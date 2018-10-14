#pragma once

#include "vector.h"
#include "vertex.h"
#include "helper.h"

// A triangle, comprised of three vertices.

struct triangle
{
	vertex a;
	vertex b;
	vertex c;

	unsigned int color;

	triangle()
	{
		a = vertex();
		b = vertex();
		c = vertex();
	}

	triangle(vector _a, vector _b, vector _c)
	{
		a = vert(_a);
		b = vert(_b);
		c = vert(_c);
	}

	triangle(vertex _a, vertex _b, vertex _c)
	{
		a = _a;
		b = _b;
		c = _c;
	}

	inline vector normal()
	{
		vector t_normal;

		vector t_line1;
		vector t_line2;

		t_line1.x = b.x - a.x;
		t_line1.y = b.y - a.y;
		t_line1.z = b.z - a.z;

		t_line2.x = c.x - a.x;
		t_line2.y = c.y - a.y;
		t_line2.z = c.z - a.z;

		t_normal.x = t_line1.y * t_line2.z - t_line1.z * t_line2.y;
		t_normal.y = t_line1.z * t_line2.x - t_line1.x * t_line2.z;
		t_normal.z = t_line1.x * t_line2.y - t_line1.y * t_line2.x;

		return t_normal.normalize();
	}
};

// Intersect a vector with a plane, and return the point where it intersects the plane. This 
// function is used for clipping.

inline vector vpintersect(vector& plane_n, vertex& l_s, vertex& l_e)
{
	double ad = dot(vector(l_s.x, l_s.y, l_s.z), plane_n);
	double bd = dot(vector(l_e.x, l_e.y, l_e.z), plane_n);

	double t = (-ad) / (bd - ad);

	vector la = vector
	(
		l_e.x - l_s.x,
		l_e.y - l_s.y,
		l_e.z - l_s.z
	);

	vector lb = vector
	(
		la.x * t,
		la.y * t,
		la.z * t
	);

	return vector
	(
		l_s.x + lb.x,
		l_s.y + lb.y,
		l_s.z + lb.z
	);
}

// Returns the shortest (signed) distance from a point to a plane, the plane normal must be 
// normalized. Don't use this. It's only for the clipping function.

inline double ppdist(vertex& p1, vector& p2)
{
	double l = sqrtf(p1.x * p1.x + p1.y * p1.y + p1.z * p1.z);

	vector p0;

	p0.x = p1.x / l;
	p0.y = p1.y / l;
	p0.z = p1.z / l;

	return p0.x * p2.x + p0.y * p2.y + p0.z * p2.z;
}

// Clip a triangle against a plane. If the triangle lies completely outside the plane, it can be
// rejected. If the triangle lies completely inside the plane, it can be left unmodified.
// Otherwise, it is divided into two triangles (which form a quad), or one new triangle.

inline int clip(vector plane_n, triangle &t_in, triangle &t_out1, triangle &t_out2)
{
	plane_n = plane_n.normalize();

	vertex* p_inside[3];
	vertex* p_outside[3];

	int c_inside = 0;
	int c_outside = 0;

	double d0 = ppdist(t_in.a, plane_n);
	double d1 = ppdist(t_in.b, plane_n);
	double d2 = ppdist(t_in.c, plane_n);

	// Plot the vertices of the input triangle into either the inside array or the outside array.
	// To check whether a vertex is inside, check if it's distance from the plane is greater or
	// equal to zero.

	if (d0 >= 0)
	{
		p_inside[c_inside++] = &t_in.a;
	}
	else
	{
		p_outside[c_outside++] = &t_in.a;
	}

	if (d1 >= 0)
	{
		p_inside[c_inside++] = &t_in.b;
	}
	else
	{
		p_outside[c_outside++] = &t_in.b;
	}

	if (d2 >= 0)
	{
		p_inside[c_inside++] = &t_in.c;
	}
	else
	{
		p_outside[c_outside++] = &t_in.c;
	}

	// Now do simple checks to see if a triangle is completely outside the plane, or if a triangle
	// is completely inside the plane, or if it is clipped by the plane.

	if (c_inside == 0)
	{
		return 0;
	}

	if (c_inside == 3)
	{
		t_out1 = t_in;

		return 1;
	}

	if (c_inside == 1 && c_outside == 2)
	{
		// Two points lie outside the plane, and one point lies on the inside of the plane. This
		// means that the triangle becoes a smaller triangle.

		t_out1.color = t_in.color;

		t_out1.a = *p_inside[0];

		t_out1.b = vert(vpintersect(plane_n, *p_inside[0], *p_outside[0]));
		t_out1.c = vert(vpintersect(plane_n, *p_inside[0], *p_outside[1]));

		return 1;
	}

	if (c_inside == 2 && c_outside == 1)
	{
		// Two points lie inside the plane, and one point lies outside the plane. This means that
		// the triangle becomes a quad made of two triangles.

		t_out1.color = t_in.color;
		t_out2.color = t_in.color;

		t_out1.a = *p_inside[0];
		t_out1.b = *p_inside[1];
		t_out2.a = *p_inside[1];

		t_out1.c = vert(vpintersect(plane_n, *p_inside[0], *p_outside[0]));
		t_out2.c = vert(vpintersect(plane_n, *p_inside[1], *p_outside[0]));

		t_out2.b = t_out1.c;

		return 2;
	}

	return 0;
}