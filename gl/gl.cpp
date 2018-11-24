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
