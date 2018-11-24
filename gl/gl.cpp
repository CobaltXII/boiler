// Software graphics library? Awesome.

#include "../boiler/boiler.h"

#include <list>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>

// Three-dimensional vector.

struct vec3
{
	double x;
	double y;
	double z;
	double w;

	vec3(double _x = 0.0, double _y = 0.0, double _z = 0.0, double _w = 1.0)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
};

// Add two vectors.

vec3 vec_add(vec3 a, vec3 b)
{
	return vec3
	(
		a.x + b.x,
		a.y + b.y, 
		a.z + b.z
	);
}

// Subtract two vectors.

vec3 vec_subtract(vec3 a, vec3 b)
{
	return vec3
	(
		a.x - b.x,
		a.y - b.y, 
		a.z - b.z
	);
}

// Multiply two vectors.

vec3 vec_multiply(vec3 a, vec3 b)
{
	return vec3
	(
		a.x * b.x,
		a.y * b.y, 
		a.z * b.z
	);
}

// Divide two vectors.

vec3 vec_divide(vec3 a, vec3 b)
{
	return vec3
	(
		a.x / b.x,
		a.y / b.y, 
		a.z / b.z
	);
}

// Multiply a vector and a scalar.

vec3 vec_multiply(vec3 a, double s)
{
	return vec3
	(
		a.x * s,
		a.y * s, 
		a.z * s
	);
}

// Divide a vector and a scalar.

vec3 vec_divide(vec3 a, double s)
{
	return vec3
	(
		a.x / s,
		a.y / s, 
		a.z / s
	);
}

// Normalize vector.

vec3 normalize(vec3 i)
{
	vec3 o;

	double l = sqrt
	(
		i.x * i.x +
		i.y * i.y +
		i.z * i.z
	);

	o.x = i.x / l;
	o.y = i.y / l;
	o.z = i.z / l;

	return o;
}

// Cross product.

vec3 cross(vec3 v1, vec3 v2)
{
	vec3 v;

	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;

	return v;
}

// Dot product.

double dot(vec3 a, vec3 b)
{
	return
	(
		a.x * b.x +
		a.y * b.y +
		a.z * b.z
	);
}

// Vector intersect plane.

vec3 intersect(vec3 plane_p, vec3 plane_n, vec3 line_a, vec3 line_b)
{
	plane_n = normalize(plane_n);

	double plane_d = -dot(plane_n, plane_p);

	double ad = dot(line_a, plane_n);
	double bd = dot(line_b, plane_n);

	double t = (-plane_d - ad) / (bd - ad);

	vec3 line_d = vec_subtract(line_b, line_a);

	vec3 line_i = vec_multiply(line_d, t);

	return vec_add(line_a, line_i);
}

// Get 'right' vector.

vec3 vec_right(vec3 pos, vec3 target, vec3 up)
{
	vec3 new_forward = normalize(vec_subtract(target, pos));

	vec3 a = vec_multiply(new_forward, dot(up, new_forward));

	vec3 new_up = normalize(vec_subtract(up, a));

	vec3 new_right = cross(new_up, new_forward);

	return new_right;
}

// Shortest distance from point to plane.

double clip_dist(vec3 p, vec3 plane_p, vec3 plane_n)
{
	vec3 n = normalize(p);

	return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dot(plane_n, plane_p));
}

// Triangle.

struct triangle
{
	vec3 p[3];

	unsigned int color;

	triangle()
	{
		p[0] = vec3();
		p[1] = vec3();
		p[2] = vec3();
	}

	triangle(vec3 a, vec3 b, vec3 c)
	{
		p[0] = a;
		p[1] = b;
		p[2] = c;
	}
};

// Get surface normal of triangle.

vec3 surface_normal(triangle t)
{
	vec3 normal;

	vec3 line1;
	vec3 line2;

	line1.x = t.p[1].x - t.p[0].x;
	line1.y = t.p[1].y - t.p[0].y;
	line1.z = t.p[1].z - t.p[0].z;

	line2.x = t.p[2].x - t.p[0].x;
	line2.y = t.p[2].y - t.p[0].y;
	line2.z = t.p[2].z - t.p[0].z;

	normal.x = line1.y * line2.z - line1.z * line2.y;
	normal.y = line1.z * line2.x - line1.x * line2.z;
	normal.z = line1.x * line2.y - line1.y * line2.x;

	normal = normalize(normal);

	return normal;
}

// Clip triangle against plane.

int clip(vec3 plane_p, vec3 plane_n, triangle in_tri, triangle& out_tri1, triangle& out_tri2)
{
	plane_n = normalize(plane_n);

	vec3* i_points[3];
	vec3* o_points[3];

	int ip_count = 0;
	int op_count = 0;

	double d0 = clip_dist(in_tri.p[0], plane_p, plane_n);
	double d1 = clip_dist(in_tri.p[1], plane_p, plane_n);
	double d2 = clip_dist(in_tri.p[2], plane_p, plane_n);

	if (d0 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[0];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[0];
	}

	if (d1 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[1];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[1];
	}

	if (d2 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[2];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[2];
	}

	if (ip_count == 0)
	{
		return 0;
	}
	else if (ip_count == 3)
	{
		out_tri1 = in_tri;

		return 1;
	}
	else if (ip_count == 1 && op_count == 2)
	{
		out_tri1.color = in_tri.color;

		out_tri1.p[0] = *i_points[0];

		out_tri1.p[1] = intersect(plane_p, plane_n, *i_points[0], *o_points[0]);
		out_tri1.p[2] = intersect(plane_p, plane_n, *i_points[0], *o_points[1]);

		return 1;
	}
	else
	{
		out_tri1.color = in_tri.color;
		out_tri2.color = in_tri.color;

		out_tri1.p[0] = *i_points[0];
		out_tri1.p[1] = *i_points[1];

		out_tri1.p[2] = intersect(plane_p, plane_n, *i_points[0], *o_points[0]);

		out_tri2.p[0] = *i_points[1];

		out_tri2.p[1] = out_tri1.p[2];

		out_tri2.p[2] = intersect(plane_p, plane_n, *i_points[1], *o_points[0]);

		return 2;
	}
}
