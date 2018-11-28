// Software graphics library? Awesome.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <list>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>

std::string path_to_obj = "teapot.obj";

std::string path_to_tex;

// String split routines.

std::vector<std::string> split(std::string s, std::string delim) 
{
    std::vector<std::string> elems;

    std::string elem;

    for (int i = 0; i < s.size(); i++)
    {
    	if (delim.find(s[i]) == std::string::npos)
    	{
    		// Found non-whitespace.

    		elem.push_back(s[i]);
    	}
    	else if (elem.size() != 0)
    	{
    		// Found whitespace, and have an element.

    		elems.push_back(elem);

    		elem.clear();
    	}
    }

    if (elem.size() != 0)
    {
    	elems.push_back(elem);
    }

    return elems;
}

// Two-dimensional vector for texture coordinates.

struct vec2
{
	double u;
	double v;
	double w;

	vec2(double _u = 0.0, double _v = 0.0, double _w = 1.0)
	{
		u = _u;
		v = _v;
		w = _w;
	}
};

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

vec3 intersect(vec3 plane_p, vec3 plane_n, vec3 line_a, vec3 line_b, double& t)
{
	plane_n = normalize(plane_n);

	double plane_d = -dot(plane_n, plane_p);

	double ad = dot(line_a, plane_n);
	double bd = dot(line_b, plane_n);

	t = (-plane_d - ad) / (bd - ad);

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
	vec2 t[3];

	unsigned int color;

	triangle()
	{
		p[0] = vec3();
		p[1] = vec3();
		p[2] = vec3();

		t[0] = vec2();
		t[1] = vec2();
		t[2] = vec2();
	}

	triangle(vec3 a, vec3 b, vec3 c)
	{
		p[0] = a;
		p[1] = b;
		p[2] = c;

		t[0] = vec2();
		t[1] = vec2();
		t[2] = vec2();
	}

	triangle(vec3 a, vec3 b, vec3 c, vec2 ta, vec2 tb, vec2 tc)
	{
		p[0] = a;
		p[1] = b;
		p[2] = c;

		t[0] = ta;
		t[1] = tb;
		t[2] = tc;
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

	vec2* i_tex[3];
	vec2* o_tex[3];

	int ip_count = 0;
	int op_count = 0;

	int it_count = 0;
	int ot_count = 0;

	double d0 = clip_dist(in_tri.p[0], plane_p, plane_n);
	double d1 = clip_dist(in_tri.p[1], plane_p, plane_n);
	double d2 = clip_dist(in_tri.p[2], plane_p, plane_n);

	if (d0 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[0];

		i_tex[it_count++] = &in_tri.t[0];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[0];

		o_tex[ot_count++] = &in_tri.t[0];
	}

	if (d1 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[1];

		i_tex[it_count++] = &in_tri.t[1];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[1];

		o_tex[ot_count++] = &in_tri.t[1];
	}

	if (d2 >= 0)
	{
		i_points[ip_count++] = &in_tri.p[2];

		i_tex[it_count++] = &in_tri.t[2];
	}
	else
	{
		o_points[op_count++] = &in_tri.p[2];

		o_tex[ot_count++] = &in_tri.t[2];
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

		out_tri1.t[0] = *i_tex[0];

		double t;

		out_tri1.p[1] = intersect(plane_p, plane_n, *i_points[0], *o_points[0], t);

		out_tri1.t[1].u = t * (o_tex[0]->u - i_tex[0]->u) + i_tex[0]->u;
		out_tri1.t[1].v = t * (o_tex[0]->v - i_tex[0]->v) + i_tex[0]->v;
		out_tri1.t[1].w = t * (o_tex[0]->w - i_tex[0]->w) + i_tex[0]->w;

		out_tri1.p[2] = intersect(plane_p, plane_n, *i_points[0], *o_points[1], t);

		out_tri1.t[2].u = t * (o_tex[1]->u - i_tex[0]->u) + i_tex[0]->u;
		out_tri1.t[2].v = t * (o_tex[1]->v - i_tex[0]->v) + i_tex[0]->v;
		out_tri1.t[2].w = t * (o_tex[1]->w - i_tex[0]->w) + i_tex[0]->w;

		return 1;
	}
	else
	{
		out_tri1.color = in_tri.color;
		out_tri2.color = in_tri.color;

		out_tri1.p[0] = *i_points[0];
		out_tri1.p[1] = *i_points[1];

		out_tri1.t[0] = *i_tex[0];
		out_tri1.t[1] = *i_tex[1];

		double t;

		out_tri1.p[2] = intersect(plane_p, plane_n, *i_points[0], *o_points[0], t);

		out_tri1.t[2].u = t * (o_tex[0]->u - i_tex[0]->u) + i_tex[0]->u;
		out_tri1.t[2].v = t * (o_tex[0]->v - i_tex[0]->v) + i_tex[0]->v;
		out_tri1.t[2].w = t * (o_tex[0]->w - i_tex[0]->w) + i_tex[0]->w;

		out_tri2.p[0] = *i_points[1];

		out_tri2.t[0] = *i_tex[1];

		out_tri2.p[1] = out_tri1.p[2];

		out_tri2.t[1] = out_tri1.t[2];

		out_tri2.p[2] = intersect(plane_p, plane_n, *i_points[1], *o_points[0], t);

		out_tri2.t[2].u = t * (o_tex[0]->u - i_tex[1]->u) + i_tex[1]->u;
		out_tri2.t[2].v = t * (o_tex[0]->v - i_tex[1]->v) + i_tex[1]->v;
		out_tri2.t[2].w = t * (o_tex[0]->w - i_tex[1]->w) + i_tex[1]->w;

		return 2;
	}
}

// Mesh comprised of triangles.

struct mesh
{
	std::vector<triangle> t;

	vec3 center;

	mesh(std::vector<triangle> _t = std::vector<triangle>())
	{
		t = _t;
	}
};

// Load mesh from file.

mesh load_obj(std::string path, bool& textured)
{
	mesh output;

	std::ifstream file = std::ifstream(path);

	if (!file.is_open())
	{
		// Can't open it, scream.

		nuke("Could not open object file \"" + path + "\".");
	}

	std::vector<vec3> vertices;
	std::vector<vec2> textures;

	textured = false;

	while (file.eof() == false)
	{
		std::string line;

		std::getline(file, line);

		std::stringstream string = std::stringstream(line);

		if (line.size() == 0 || line[0] == '#' || line.find_first_not_of(" \f\n\r\t\v") == std::string::npos)
		{
			// Comment or whitespace.

			continue;
		}

		std::string command;

		for (int i = 0; i < line.size(); i++)
		{
			if (std::string(" \f\n\r\t\v").find(line[i]) == std::string::npos)
			{
				// Not whitespace.

				command.push_back(line[i]);
			}
			else
			{
				// Whitespace, flush.

				break;
			}
		}

		if (command == "v")
		{
			// Vertex.

			string = std::stringstream(line.substr(1, line.size() - 1));

			double x;
			double y;
			double z;

			string >> x >> y >> z;

			vertices.push_back(vec3(x, y, z));
		}
		else if (command == "vt")
		{
			// Texture coordinate.

			textured = true;

			string = std::stringstream(line.substr(2, line.size() - 2));

			double u;
			double v;

			string >> u >> v;

			textures.push_back(vec2(u, v));
		}
		else if (command == "f")
		{
			// Face.

			std::string datum = line.substr(1, line.size() - 1);

			std::size_t not_whitespace = datum.find_first_not_of(" \f\n\r\t\v");

			datum = datum.substr(not_whitespace, datum.size() - not_whitespace);

			std::vector<std::string> all_indices = split(datum, " \f\n\r\t\v");

			std::vector<int> p_indices;
			std::vector<int> t_indices;

			for (int i = 0; i < all_indices.size(); i++)
			{
				std::vector<std::string> info = split(all_indices[i], "/");

				p_indices.push_back(std::stoi(info[0]));

				if (textured)
				{
					t_indices.push_back(std::stoi(info[1]));
				}
			}

			for (int i = 2; i < p_indices.size(); i++)
			{
				if (textured)
				{
					output.t.push_back(triangle
					(
						vertices[p_indices[0] - 1],

						vertices[p_indices[i - 1] - 1],
						vertices[p_indices[i - 0] - 1],

						textures[t_indices[0] - 1],

						textures[t_indices[i - 1] - 1],
						textures[t_indices[i - 0] - 1]
					));
				}
				else
				{
					output.t.push_back(triangle
					(
						vertices[p_indices[0] - 1],

						vertices[p_indices[i - 1] - 1],
						vertices[p_indices[i - 0] - 1]
					));
				}
			}
		}
	}

	// Calculating center of mass.

	for (int i = 0; i < vertices.size(); i++)
	{
		output.center.x -= vertices[i].x / double(vertices.size());
		output.center.y -= vertices[i].y / double(vertices.size());
		output.center.z -= vertices[i].z / double(vertices.size());
	}

	// Find greatest length from origin.

	double max_length = 0.0;

	for (int i = 0; i < vertices.size(); i++)
	{
		vec3 offset = vec_add(vertices[i], output.center);

		double length = sqrt
		(
			offset.x * offset.x + 
			offset.y * offset.y + 
			offset.z * offset.z
		);

		if (length > max_length)
		{
			max_length = length;
		}
	}

	// Scale so that maximum length is set.

	double set_length = 10.0;

	double set_factor = max_length / set_length;

	for (int i = 0; i < output.t.size(); i++)
	{
		output.t[i].p[0] = vec_subtract(vec_divide(vec_add(output.t[i].p[0], output.center), set_factor), output.center);
		output.t[i].p[1] = vec_subtract(vec_divide(vec_add(output.t[i].p[1], output.center), set_factor), output.center);
		output.t[i].p[2] = vec_subtract(vec_divide(vec_add(output.t[i].p[2], output.center), set_factor), output.center);
	}

	std::cout << vertices.size() << " vertices, " << output.t.size() << " triangles." << std::endl;

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

	double w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

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
		multiply(i.p[2], m),

		i.t[0],
		i.t[1],
		i.t[2]
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
	double z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0;
	double z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0;

	return z1 > z2;
}

// Boiler framework.

struct game: boiler
{	
	// Sample texture.

	inline unsigned int sample(double u, double v, unsigned int* tex, int tex_w, int tex_h)
	{
		int x = u * tex_w;
		int y = v * tex_h;

		if (x >= tex_w || y >= tex_h || x < 0 || y < 0)
		{
			return 0x00000000;
		}
		else
		{
			return tex[y * tex_w + x];
		}
	}

	// Perspective-corrected textured triangle rasterizer.

	void tex_triangle
	(
		int x1, int y1, double u1, double v1, double w1,
		int x2, int y2, double u2, double v2, double w2,
		int x3, int y3, double u3, double v3, double w3,

		unsigned int* dtex,

		int dtex_w,
		int dtex_h,

		double shading
	)
	{
		if (y2 < y1)
		{
			std::swap(y1, y2);
			std::swap(x1, x2);
			std::swap(u1, u2);
			std::swap(v1, v2);
			std::swap(w1, w2);
		}

		if (y3 < y1)
		{
			std::swap(y1, y3);
			std::swap(x1, x3);
			std::swap(u1, u3);
			std::swap(v1, v3);
			std::swap(w1, w3);
		}

		if (y3 < y2)
		{
			std::swap(y2, y3);
			std::swap(x2, x3);
			std::swap(u2, u3);
			std::swap(v2, v3);
			std::swap(w2, w3);
		}

		int dy1 = y2 - y1;
		int dx1 = x2 - x1;

		double dv1 = v2 - v1;
		double du1 = u2 - u1;
		double dw1 = w2 - w1;

		int dy2 = y3 - y1;
		int dx2 = x3 - x1;

		double dv2 = v3 - v1;
		double du2 = u3 - u1;
		double dw2 = w3 - w1;

		double tex_u;
		double tex_v;
		double tex_w;

		double dax_step = 0.0;
		double dbx_step = 0.0;

		double du1_step = 0.0;
		double dv1_step = 0.0;
		double du2_step = 0.0;
		double dv2_step = 0.0;
		double dw1_step = 0.0;
		double dw2_step = 0.0;

		if (dy1)
		{
			dax_step = dx1 / (double)abs(dy1);
		}

		if (dy2)
		{
			dbx_step = dx2 / (double)abs(dy2);
		}

		if (dy1)
		{
			du1_step = du1 / (double)abs(dy1);
			dv1_step = dv1 / (double)abs(dy1);
			dw1_step = dw1 / (double)abs(dy1);
		}

		if (dy2)
		{
			du2_step = du2 / (double)abs(dy2);
			dv2_step = dv2 / (double)abs(dy2);
			dw2_step = dw2 / (double)abs(dy2);
		}

		if (dy1)
		{
			for (int i = y1; i <= y2; i++)
			{
				int ax = x1 + (double)(i - y1) * dax_step;
				int bx = x1 + (double)(i - y1) * dbx_step;

				double tex_su = u1 + (double)(i - y1) * du1_step;
				double tex_sv = v1 + (double)(i - y1) * dv1_step;
				double tex_sw = w1 + (double)(i - y1) * dw1_step;

				double tex_eu = u1 + (double)(i - y1) * du2_step;
				double tex_ev = v1 + (double)(i - y1) * dv2_step;
				double tex_ew = w1 + (double)(i - y1) * dw2_step;

				if (ax > bx)
				{
					std::swap(ax, bx);

					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				double tstep = 1.0 / ((double)(bx - ax));

				double t = 0.0;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0 - t) * tex_su + t * tex_eu;
					tex_v = (1.0 - t) * tex_sv + t * tex_ev;
					tex_w = (1.0 - t) * tex_sw + t * tex_ew;

					unsigned int cs = sample(tex_u / tex_w, tex_v / tex_w, dtex, dtex_w, dtex_h);

					double rs = double(mgetr(cs)) * shading;
					double gs = double(mgetg(cs)) * shading;
					double bs = double(mgetb(cs)) * shading;

					plotp(j, i, mrgb(mclamprgb(rs), mclamprgb(gs), mclamprgb(bs)));

					t += tstep;
				}
			}
		}

		dy1 = y3 - y2;
		dx1 = x3 - x2;
		dv1 = v3 - v2;
		du1 = u3 - u2;
		dw1 = w3 - w2;

		if (dy1)
		{
			dax_step = dx1 / (double)abs(dy1);
		}

		if (dy2) 
		{
			dbx_step = dx2 / (double)abs(dy2);
		}

		du1_step = 0;
		dv1_step = 0;

		if (dy1)
		{
			du1_step = du1 / (double)abs(dy1);
			dv1_step = dv1 / (double)abs(dy1);
			dw1_step = dw1 / (double)abs(dy1);
		}

		if (dy1)
		{
			for (int i = y2; i <= y3; i++)
			{
				int ax = x2 + (double)(i - y2) * dax_step;
				int bx = x1 + (double)(i - y1) * dbx_step;

				double tex_su = u2 + (double)(i - y2) * du1_step;
				double tex_sv = v2 + (double)(i - y2) * dv1_step;
				double tex_sw = w2 + (double)(i - y2) * dw1_step;

				double tex_eu = u1 + (double)(i - y1) * du2_step;
				double tex_ev = v1 + (double)(i - y1) * dv2_step;
				double tex_ew = w1 + (double)(i - y1) * dw2_step;

				if (ax > bx)
				{
					std::swap(ax, bx);

					std::swap(tex_su, tex_eu);
					std::swap(tex_sv, tex_ev);
					std::swap(tex_sw, tex_ew);
				}

				tex_u = tex_su;
				tex_v = tex_sv;
				tex_w = tex_sw;

				double tstep = 1.0 / ((double)(bx - ax));

				double t = 0.0;

				for (int j = ax; j < bx; j++)
				{
					tex_u = (1.0 - t) * tex_su + t * tex_eu;
					tex_v = (1.0 - t) * tex_sv + t * tex_ev;
					tex_w = (1.0 - t) * tex_sw + t * tex_ew;

					unsigned int cs = sample(tex_u / tex_w, tex_v / tex_w, dtex, dtex_w, dtex_h);

					double rs = double(mgetr(cs)) * shading;
					double gs = double(mgetg(cs)) * shading;
					double bs = double(mgetb(cs)) * shading;

					plotp(j, i, mrgb(mclamprgb(rs), mclamprgb(gs), mclamprgb(bs)));

					t += tstep;
				}
			}
		}
	}

	// Mesh to render.

	mesh obj;

	bool obj_textured;

	int spin_mode = 0;

	double obj_rot_x = 0.0;
	double obj_rot_y = 0.0;
	double obj_rot_z = 0.0;

	double obj_rot_xv = 0.0;
	double obj_rot_yv = 0.0;
	double obj_rot_zv = 0.0;

	// Projection matrix.

	mat4 projection = mat_projection(0.128, 1024.0, 90.0, 1.0);

	// Camera.

	vec3 camera_p = vec3(0.0, 0.0, -15.0);

	vec3 camera_d = vec3(0.0, 0.0, 1.0);

	// Texture.

	int gdtex_w;
	int gdtex_h;

	unsigned int* gdtex;

	// Initializer.

	void steam() override
	{
		width = 800;
		height = 600;

		title = "GL (using Boiler)";

		obj = load_obj(path_to_obj, obj_textured);

		if (obj_textured)
		{
			if (path_to_tex.size() == 0)
			{
				std::cout << "This model can be textured, consider passing a second argument for the texture path." << std::endl;

				obj_textured = false;
			}
			else
			{
				gdtex = loadimg(path_to_tex, gdtex_w, gdtex_h);

				if (!gdtex)
				{
					nuke("Could not load texture \"" + path_to_tex + "\".");
				}
			}
		}

		if (false)
		{
			// Generate cube.

			obj.t =
			{
				triangle(vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0)),
				triangle(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0)),
	                                      
				triangle(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0)),
				triangle(vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0)),
	                                                  
				triangle(vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0)),
				triangle(vec3(1.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), vec3(0.0, 0.0, 1.0)),
	                                                     
				triangle(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0)),
				triangle(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 0.0)),
	                                                    
				triangle(vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 1.0), vec3(1.0, 1.0, 1.0)),
				triangle(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 1.0, 0.0)),
	                                                
				triangle(vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 0.0)),
				triangle(vec3(1.0, 0.0, 1.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0))
			};
		}
	}

	// Keypress handler.

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_ESCAPE)
		{
			nuke();
		}
		else if (e.key.keysym.sym == SDLK_1)
		{
			spin_mode = 0;
		}
		else if (e.key.keysym.sym == SDLK_2)
		{
			spin_mode = 1;
		}
		else if (e.key.keysym.sym == SDLK_3)
		{
			spin_mode = 2;
		}
		else if (e.key.keysym.sym == SDLK_4)
		{
			spin_mode = 3;
		}
		else if (e.key.keysym.sym == SDLK_5)
		{
			spin_mode = 4;
		}
		else if (e.key.keysym.sym == SDLK_z)
		{
			obj_rot_x = 0.0;
			obj_rot_y = 0.0;
			obj_rot_z = 0.0;
		}
	}

	// Frame.

	void draw() override
	{
		black();

		// Looking.

		double x_fov = 90.0;
		double y_fov = 45.0;

		mat4 cam_rot_x = mat_rot_x(0.0);
		mat4 cam_rot_y = mat_rot_y(0.0);

		// Setup world/view/camera.

		vec3 vec_up = vec3(0.0, 1.0, 0.0);

		vec3 vec_target = vec3(0.0, 0.0, 1.0);

		camera_d = multiply(multiply(vec_target, cam_rot_x), cam_rot_y);

		vec_target = vec_add(camera_p, camera_d);

		mat4 mat_camera = mat_point_at(camera_p, vec_target, vec_up);

		mat4 mat_view = mat_inverse(mat_camera);

		// Handle keypresses.

		{
			const Uint8* keys = SDL_GetKeyboardState(NULL);

			double f = 0.65;

			// Movement.

			if (keys[SDL_SCANCODE_S])
			{
				camera_p = vec_subtract(camera_p, vec_multiply(camera_d, f));
			}
			else if (keys[SDL_SCANCODE_W])
			{
				camera_p = vec_add(camera_p, vec_multiply(camera_d, f));
			}

			vec3 v_right = vec_right(camera_p, vec_target, vec_up);

			if (keys[SDL_SCANCODE_D])
			{
				camera_p = vec_subtract(camera_p, vec_multiply(v_right, f));
			}
			else if (keys[SDL_SCANCODE_A])
			{
				camera_p = vec_add(camera_p, vec_multiply(v_right, f));
			}

			if (keys[SDL_SCANCODE_N])
			{
				camera_p = vec_subtract(camera_p, vec_multiply(vec_up, f));
			}
			else if (keys[SDL_SCANCODE_M])
			{
				camera_p = vec_add(camera_p, vec_multiply(vec_up, f));
			}
		}

		// Preprocess triangles.

		std::vector<triangle> rasterize;

		for (int i = 0; i < obj.t.size(); i++)
		{
			triangle t_original = obj.t[i];

			// Do transformations.

			{
				// Fix origin in center of object.

				t_original = multiply(t_original, mat_translate(obj.center));

				// Rotate object.

				double slow = 1.0 / 3.0;

				if (spin_mode == 0)
				{
					obj_rot_x = iteration / 26.0 * slow;
					obj_rot_y = iteration / 22.0 * slow;
					obj_rot_z = iteration / 38.0 * slow;
				}

				t_original = multiply(t_original, mat_rot_x(obj_rot_x));
				t_original = multiply(t_original, mat_rot_y(obj_rot_y));
				t_original = multiply(t_original, mat_rot_z(obj_rot_z));

				// Scale object.

				double s = 1.0;

				t_original = multiply(t_original, mat_scalar(vec3(s, s, s)));
			}

			// Get surface normal.

			vec3 t_normal = surface_normal(t_original);

			// Get ray from triangle to camera.

			vec3 camera_ray = vec_subtract(t_original.p[0], camera_p);

			if (backface_cull(t_normal, camera_ray))
			{
				// Illumination.

				vec3 light_direction = normalize(vec3(0.0, 0.0, -1.0));

				double light_dot = dot(t_normal, light_direction);

				// World space to view space.

				t_original = multiply(t_original, mat_view);
				
				// Clip against near plane.

				triangle t_clipped[2];

				int num_clipped = clip(vec3(0.0, 0.0, 0.128), vec3(0.0, 0.0, 1.0), t_original, t_clipped[0], t_clipped[1]);

				for (int j = 0; j < num_clipped; j++)
				{
					t_original = t_clipped[j];

					// Project triangle.

					triangle t_projected = multiply(t_original, projection);

					// Scale into view. Additional math to preserve 1:1 aspect ratio.

					t_projected.p[0] = vec_divide(t_projected.p[0], t_projected.p[0].w);
					t_projected.p[1] = vec_divide(t_projected.p[1], t_projected.p[1].w);
					t_projected.p[2] = vec_divide(t_projected.p[2], t_projected.p[2].w);

					t_projected.p[0].x *= -1.0;
					t_projected.p[1].x *= -1.0;
					t_projected.p[2].x *= -1.0;

					t_projected.p[0].y *= -1.0;
					t_projected.p[1].y *= -1.0;
					t_projected.p[2].y *= -1.0;

					t_projected.p[0].x = (t_projected.p[0].x + 1.0) * (0.5 * (double)height) + (double)(width - height) / 2;
					t_projected.p[1].x = (t_projected.p[1].x + 1.0) * (0.5 * (double)height) + (double)(width - height) / 2;
					t_projected.p[2].x = (t_projected.p[2].x + 1.0) * (0.5 * (double)height) + (double)(width - height) / 2;

					t_projected.p[0].y = (t_projected.p[0].y + 1.0) * (0.5 * (double)height);
					t_projected.p[1].y = (t_projected.p[1].y + 1.0) * (0.5 * (double)height);
					t_projected.p[2].y = (t_projected.p[2].y + 1.0) * (0.5 * (double)height);

					double ambient = 30.0;

					if (obj_textured)
					{
						t_projected.color = (16.0 + light_dot) * 512.0;
					}
					else
					{
						t_projected.color = rgb
						(
							clamprgb(std::max(0.0, (-t_normal.x + 1.0) / 2.0 * light_dot * 255.0) + ambient), 
							clamprgb(std::max(0.0, (-t_normal.y + 1.0) / 2.0 * light_dot * 255.0) + ambient), 
							clamprgb(std::max(0.0, (-t_normal.z + 1.0) / 2.0 * light_dot * 255.0) + ambient)
						);
					}

					// t_projected.color = rgb
					// (
					// 	clamprgb(std::max(0.0, light_dot * 255.0) + ambient), 
					// 	clamprgb(std::max(0.0, light_dot * 255.0) + ambient), 
					// 	clamprgb(std::max(0.0, light_dot * 255.0) + ambient)
					// );

					// Push to rasterizer.

					rasterize.push_back(t_projected);
				}
			}
		}

		std::sort(rasterize.begin(), rasterize.end(), painters_algorithm);

		// Rasterize triangles.

		for (int i = 0; i < rasterize.size(); i++)
		{
			triangle t = rasterize[i];

			triangle t_clipped[2];

			std::list<triangle> l_triangles;

			l_triangles.push_back(t);

			int num_triangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int num_add = 0;

				while (num_triangles > 0)
				{
					triangle top = l_triangles.front();

					l_triangles.pop_front();

					num_triangles--;

					switch (p)
					{
						case 0:
						{
							num_add = clip(vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), top, t_clipped[0], t_clipped[1]);

							break;
						}

						case 1:
						{
							num_add = clip(vec3(0.0, height - 1, 0.0), vec3(0.0, -1.0, 0.0), top, t_clipped[0], t_clipped[1]);

							break;
						}

						case 2:
						{
							num_add = clip(vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), top, t_clipped[0], t_clipped[1]);

							break;
						}

						case 3:
						{
							num_add = clip(vec3(width - 1, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), top, t_clipped[0], t_clipped[1]);

							break;
						}
					}

					for (int k = 0; k < num_add; k++)
					{
						l_triangles.push_back(t_clipped[k]);
					}
				}

				num_triangles = l_triangles.size();
			}

			for (auto& t: l_triangles)
			{
				if (obj_textured)
				{
					tex_triangle
					(
						t.p[0].x, t.p[0].y, t.t[0].u, t.t[0].v, t.t[0].w,
						t.p[1].x, t.p[1].y, t.t[1].u, t.t[1].v, t.t[1].w,
						t.p[2].x, t.p[2].y, t.t[2].u, t.t[2].v, t.t[2].w,

						gdtex,

						gdtex_w,
						gdtex_h,

						std::max(0.0, (double(t.color) / 512.0) - 16.0) + (30.0 / 255.0)
					);
				}
				else
				{
					ftrianglergb
					(
						t.p[0].x, t.p[0].y,
						t.p[1].x, t.p[1].y,
						t.p[2].x, t.p[2].y,

						t.color
					);
				}
			}
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 1)
	{
		if (argc == 2)
		{
			path_to_obj = std::string(argv[1]);
		}
		else if (argc == 3)
		{
			path_to_obj = std::string(argv[1]);
			path_to_tex = std::string(argv[2]);
		}
		else
		{
			nuke("Usage: ./gl [path-to-.obj] [path-to-tex]");
		}
	}

	game demo;

	if (demo.make() != 0)
	{
		std::cout << "Could not initialize Boiler." << std::endl;

		return 1;
	}

	demo.engine();
	demo.sweep();

	return 0;
}