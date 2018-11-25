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

// Mesh comprised of triangles.

struct mesh
{
	std::vector<triangle> t;

	mesh(std::vector<triangle> _t = std::vector<triangle>())
	{
		t = _t;
	}
};

// Load mesh from file.

mesh load_obj(std::string path)
{
	mesh output;

	std::ifstream file = std::ifstream(path);

	if (file.is_open() == false)
	{
		return mesh();
	}

	std::vector<vec3> vertices;

	while (file.eof() == false)
	{
		std::string line;

		std::getline(file, line);

		std::stringstream string = std::stringstream(line);

		if (line[0] == 'v')
		{
			string = std::stringstream(line.substr(1, line.size() - 1));

			double x;
			double y;
			double z;

			string >> x >> y >> z;

			vertices.push_back(vec3(x, y, z));
		}
		else if (line[0] == 'f')
		{
			string = std::stringstream(line.substr(1, line.size() - 1));

			int a;
			int b;
			int c;

			string >> a >> b >> c;

			output.t.push_back(triangle(vertices[a - 1], vertices[b - 1], vertices[c - 1]));
		}
	}

	return output;
}

// 4 by 4 matrix.

struct mat4
{
	double m[4][4];

	mat4()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m[i][j] = 0.0;
			}
		}
	}
};

// Generate projection matrix.

mat4 mat_projection(double near, double far, double fov, double aspect)
{
	mat4 out;

	double fov_rad = 1.0 / tan(fov * 0.5 / 180.0 * M_PI);

	out.m[0][0] = aspect * fov_rad;

	out.m[1][1] = fov_rad;

	out.m[2][2] = far / (far - near);

	out.m[3][2] = (-far * near) / (far - near);

	out.m[2][3] = 1.0;
	out.m[3][3] = 0.0;

	return out;
}

// Generate X rotation matrix.

mat4 mat_rot_x(double theta)
{
	mat4 out;

	out.m[0][0] = 1;
	out.m[3][3] = 1;

	out.m[1][1] = 0 + cos(theta);
	out.m[1][2] = 0 + sin(theta);
	out.m[2][1] = 0 - sin(theta);
	out.m[2][2] = 0 + cos(theta);

	return out;
}

// Generate Y rotation matrix.

mat4 mat_rot_y(double theta)
{
	mat4 out;

	out.m[1][1] = 1;
	out.m[3][3] = 1;

	out.m[0][0] = 0 + cos(theta);
	out.m[2][0] = 0 + sin(theta);
	out.m[0][2] = 0 - sin(theta);
	out.m[2][2] = 0 + cos(theta);

	return out;
}

// Generate Z rotation matrix.

mat4 mat_rot_z(double theta)
{
	mat4 out;

	out.m[2][2] = 1;
	out.m[3][3] = 1;

	out.m[0][0] = 0 + cos(theta);
	out.m[0][1] = 0 + sin(theta);
	out.m[1][0] = 0 - sin(theta);
	out.m[1][1] = 0 + cos(theta);

	return out;
}

// Generate translation matrix.

mat4 mat_translate(vec3 t)
{
	mat4 out;

	out.m[0][0] = 1;
	out.m[1][1] = 1;
	out.m[2][2] = 1;
	out.m[3][3] = 1;

	out.m[3][0] = t.x;
	out.m[3][1] = t.y;
	out.m[3][2] = t.z;

	return out;
}

// Generate scalar matrix.

mat4 mat_scalar(vec3 c)
{
	mat4 out;

	out.m[0][0] = c.x;
	out.m[1][1] = c.y;
	out.m[2][2] = c.z;

	out.m[3][3] = 1;

	return out;
}

// Generate 'point at' matrix.

mat4 mat_point_at(vec3 pos, vec3 target, vec3 up)
{
	vec3 new_forward = normalize(vec_subtract(target, pos));

	vec3 a = vec_multiply(new_forward, dot(up, new_forward));

	vec3 new_up = normalize(vec_subtract(up, a));

	vec3 new_right = cross(new_up, new_forward);

	mat4 matrix;

	matrix.m[0][0] = new_right.x;	
	matrix.m[0][1] = new_right.y;	
	matrix.m[0][2] = new_right.z;	

	matrix.m[1][0] = new_up.x;		
	matrix.m[1][1] = new_up.y;		
	matrix.m[1][2] = new_up.z;		

	matrix.m[2][0] = new_forward.x;	
	matrix.m[2][1] = new_forward.y;	
	matrix.m[2][2] = new_forward.z;	
	
	matrix.m[3][0] = pos.x;			
	matrix.m[3][1] = pos.y;			
	matrix.m[3][2] = pos.z;			

	matrix.m[0][3] = 0.0;
	matrix.m[1][3] = 0.0;
	matrix.m[2][3] = 0.0;

	matrix.m[3][3] = 1.0;

	return matrix;
}

// Invert matrix, only works on some matrices?

mat4 mat_inverse(mat4 m)
{
	mat4 matrix;

	matrix.m[0][0] = m.m[0][0]; 
	matrix.m[0][1] = m.m[1][0]; 
	matrix.m[0][2] = m.m[2][0]; 

	matrix.m[1][0] = m.m[0][1]; 
	matrix.m[1][1] = m.m[1][1]; 
	matrix.m[1][2] = m.m[2][1]; 

	matrix.m[2][0] = m.m[0][2]; 
	matrix.m[2][1] = m.m[1][2]; 
	matrix.m[2][2] = m.m[2][2]; 
	
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);

	matrix.m[0][3] = 0.0;
	matrix.m[1][3] = 0.0;
	matrix.m[2][3] = 0.0;

	matrix.m[3][3] = 1.0;

	return matrix;
}

// Multiply vector by matrix.

vec3 multiply(vec3 i, mat4 m)
{
	vec3 o;

	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];

	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0)
	{
		o.x /= w; 
		o.y /= w; 
		o.z /= w;
	}

	return o;
}

// Multiply triangle by matrix.

triangle multiply(triangle i, mat4 m)
{
	triangle o = triangle
	(
		multiply(i.p[0], m),
		multiply(i.p[1], m),
		multiply(i.p[2], m)
	);

	return o;
}

// Backface culling.

bool backface_cull(vec3 n, vec3 r)
{
	return 
	(
		n.x * r.x +
		n.y * r.y +
		n.z * r.z

		< 0.0
	);
}

// Painter's algorithm.

bool painters_algorithm(triangle& t1, triangle& t2)
{
	float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0;
	float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0;

	return z1 > z2;
}
