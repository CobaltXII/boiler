#pragma once

#include <cmath>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <sstream>

#include "vector.h"
#include "vertex.h"
#include "matrix.h"
#include "triangle.h"

// An object, comprised of many triangles. An object also has various transformation values which
// can be used to transform the object before it is rendered.

struct object
{
	std::vector<triangle> faces;

	double rot_x;
	double rot_y;
	double rot_z;

	vector v_origin;
	vector v_scalar;
	vector v_translate;

	matrix mrot_x = mmatrix(4, 4);
	matrix mrot_y = mmatrix(4, 4);
	matrix mrot_z = mmatrix(4, 4);

	// Transform the object by rotating first, scaling second, and translating third. This order
	// of operations is correct for most expected transformations.

	inline triangle* transform()
	{
		triangle* out = (triangle*)malloc(faces.size() * sizeof(triangle));

		mrotx(mrot_x, rot_x);
		mroty(mrot_y, rot_y);
		mrotz(mrot_z, rot_z);

		for (int i = 0; i < faces.size(); i++)
		{
			triangle tdef = faces[i];

			// Translate the object so that the vector represented by v_origin is moved to 0, 0, 
			// 0. This is done so that the rotation works correctly.

			tdef.a.x -= v_origin.x;
			tdef.a.y -= v_origin.y;
			tdef.a.z -= v_origin.z;

			tdef.b.x -= v_origin.x;
			tdef.b.y -= v_origin.y;
			tdef.b.z -= v_origin.z;

			tdef.c.x -= v_origin.x;
			tdef.c.y -= v_origin.y;
			tdef.c.z -= v_origin.z;

			triangle trot_x;
			triangle trot_xy;
			triangle trot_xyz;

			// Rotate in the x axis.

			matvec(tdef.a, trot_x.a, mrot_x);
			matvec(tdef.b, trot_x.b, mrot_x);
			matvec(tdef.c, trot_x.c, mrot_x);

			// Rotate in the y axis.

			matvec(trot_x.a, trot_xy.a, mrot_y);
			matvec(trot_x.b, trot_xy.b, mrot_y);
			matvec(trot_x.c, trot_xy.c, mrot_y);

			// Rotate in the z axis.

			matvec(trot_xy.a, trot_xyz.a, mrot_z);
			matvec(trot_xy.b, trot_xyz.b, mrot_z);
			matvec(trot_xy.c, trot_xyz.c, mrot_z);

			// Move back to the original position.

			trot_xyz.a.x += v_origin.x;
			trot_xyz.a.y += v_origin.y;
			trot_xyz.a.z += v_origin.z;

			trot_xyz.b.x += v_origin.x;
			trot_xyz.b.y += v_origin.y;
			trot_xyz.b.z += v_origin.z;

			trot_xyz.c.x += v_origin.x;
			trot_xyz.c.y += v_origin.y;
			trot_xyz.c.z += v_origin.z;

			// Scale and translate.

			trot_xyz.a.x = (trot_xyz.a.x * v_scalar.x) + v_translate.x;
			trot_xyz.a.y = (trot_xyz.a.y * v_scalar.y) + v_translate.y;
			trot_xyz.a.z = (trot_xyz.a.z * v_scalar.z) + v_translate.z;

			trot_xyz.b.x = (trot_xyz.b.x * v_scalar.x) + v_translate.x;
			trot_xyz.b.y = (trot_xyz.b.y * v_scalar.y) + v_translate.y;
			trot_xyz.b.z = (trot_xyz.b.z * v_scalar.z) + v_translate.z;

			trot_xyz.c.x = (trot_xyz.c.x * v_scalar.x) + v_translate.x;
			trot_xyz.c.y = (trot_xyz.c.y * v_scalar.y) + v_translate.y;
			trot_xyz.c.z = (trot_xyz.c.z * v_scalar.z) + v_translate.z;

			// Add to the output vector.

			out[i] = trot_xyz;
		}

		return out;
	}

	void destroy()
	{
		mdestroy(mrot_x, 4);
		mdestroy(mrot_y, 4);
		mdestroy(mrot_z, 4);
	}
};

// This function will load an object from a file. No error checking provided, so this function is
// very explosive.

object loadobj(std::string path)
{
	object output;

	std::ifstream file = std::ifstream(path);

	if (file.is_open() == false)
	{
		return object();
	}

	std::vector<vector> vertices;

	while (file.eof() == false)
	{
		std::string line;

		// Get the next line from the file.

		std::getline(file, line);

		// Create a string stream that is derived from the current line. The string stream
		// class provides easy ways to fetch various values from strings.

		std::stringstream string = std::stringstream(line);

		if (line[0] == 'v')
		{
			// Vertex information, to be stored in the vertex array.

			string = std::stringstream(line.substr(1, line.size() - 1));

			double x;
			double y;
			double z;

			string >> x >> y >> z;

			vertices.push_back(vector(x, y, z));
		}
		else if (line[0] == 'f')
		{
			// Triangle information, to be generated using the vertex array and stored in the
			// mesh.

			string = std::stringstream(line.substr(1, line.size() - 1));

			int a;
			int b;
			int c;

			string >> a >> b >> c;

			output.faces.push_back(triangle(vertices[a - 1], vertices[b - 1], vertices[c - 1]));
		}
	}

	output.v_scalar = vector(1, 1, 1);

	return output;
}