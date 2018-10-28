/*

raster.cpp

This is a software rasterizer. It is capable of rendering three-dimensional objects to the screen
as pixels; this is called rasterization. Currently, this rasterizer supports rotation, scaling,
translation, projection, and limited clipping. The clipping is very buggy, so objects must be
rendered at a distance. Texturing is not supported, objects are colored with a color derived from
their normals. Only flat shading is implemented.

I used @javidx9#3371's awesome tutorials to help me make this awesome rasterizer

	https://www.youtube.com/watch?v=nBzCS-Y0FcY&t=2385s
	https://www.youtube.com/watch?v=HXSuNxpCzdM
	https://www.youtube.com/watch?v=XgMWc6LumG4&t=1971s
	https://www.youtube.com/watch?v=ih20l3pJoeU

*/

#include "../boiler/boiler.h"

#include "include/vector.h"
#include "include/vertex.h"
#include "include/matrix.h"
#include "include/triangle.h"
#include "include/object.h"
#include "include/helper.h"

#include <cmath>
#include <cstdlib>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>

struct game: boiler
{	
	// Load an object from a file. This is the only object we will be rendering, although this
	// renderer can render much more than one object.

	object my_object = loadobj("obj/bunny.obj");

	// Generate the projection matrix. This renderer will modify the whole world relative to the
	// camera, so that the projection matrix does not need to change.

	matrix m_proj = mproj(1.0, 1024.0, 90.0);

	// This function is called by the Boiler framework to initialize the rendering target. It just
	// sets up the dimensions and title of the window.

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Software renderer (using Boiler)";
	}

	// This function is called every frame by the Boiler framework. All the rendering and
	// rasterization must happen in this function.

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Define all transformations here.

		my_object.rot_x = degrad(180.0);

		my_object.rot_y = iteration / 55.0;

		my_object.rot_z = 0.0;

		my_object.v_origin.x = 0.0;
		my_object.v_origin.y = 0.0;
		my_object.v_origin.z = 0.0;

		my_object.v_scalar.x = 6.3;
		my_object.v_scalar.y = 6.3;
		my_object.v_scalar.z = 6.3;

		my_object.v_translate.x = 0.1;
		my_object.v_translate.y = 0.6;
		my_object.v_translate.z = 1.0;

		// Transform the object and reduce it into a bag of triangles.

		triangle* obj = my_object.transform();

		// We will populate an array with the triangles that will be rastered.

		std::vector<triangle> v_raster;

		for (int i = 0; i < my_object.faces.size(); i++)
		{
			triangle t_default = obj[i];

			vector v_normal = t_default.normal();

			if (cull(v_normal, t_default))
			{
				// The object was only transformed, not projected. Now we need to project it to
				// the screen, so that it can be rasterized.

				triangle t_projected;

				matvec(t_default.a, t_projected.a, m_proj);
				matvec(t_default.b, t_projected.b, m_proj);
				matvec(t_default.c, t_projected.c, m_proj);

				// These coordinates are normalized, that is, they are in the bounds of -1 and 1. We
				// need to scale these coordinates into screen space.

				t_projected.a.x = (t_projected.a.x + 1.0) * 0.5 * (double)height + (double)(width - height) / 2;
				t_projected.b.x = (t_projected.b.x + 1.0) * 0.5 * (double)height + (double)(width - height) / 2;
				t_projected.c.x = (t_projected.c.x + 1.0) * 0.5 * (double)height + (double)(width - height) / 2;

				t_projected.a.y = (t_projected.a.y + 1.0) * 0.5 * (double)height;
				t_projected.b.y = (t_projected.b.y + 1.0) * 0.5 * (double)height;
				t_projected.c.y = (t_projected.c.y + 1.0) * 0.5 * (double)height;

				// Illumination.

				double f_illum = dot(v_normal, vector(0.0, 0.0, -1.0).normalize());

				t_projected.color = rgb
				(
					clamprgb(255.0 - (v_normal.x + 1) / 2 * 255.0 * f_illum), 
					clamprgb(255.0 - (v_normal.y + 1) / 2 * 255.0 * f_illum), 
					clamprgb(255.0 - (v_normal.z + 1) / 2 * 255.0 * f_illum)
				);

				// Add the projected triangle to the rasterizing array.

				v_raster.push_back(t_projected);
			}
		}

		std::sort(v_raster.begin(), v_raster.end(), paintersalgorithm);

		for (int i = 0; i < v_raster.size(); i++)
		{
			triangle t_projected = v_raster[i];

			// Create a list of triangles that will be used for clipping. A triangle is clipped 4
			// times against each boundary plane, and the resulting triangles are added to the
			// list.

			triangle c1;
			triangle c2;

			int c_new = 1;

			std::list<triangle> l_clipped;

			// Add the initial triangle.

			l_clipped.push_back(t_projected);

			for (int p = 0; p < 4; p++)
			{
				int c_clip = 0;

				while (c_new > 0)
				{
					triangle t_subject = l_clipped.front();

					// Remove the triangle at the front of the list from the list. This is done so
					// that the same triangle is not clipped more times than needed.

					l_clipped.pop_front();

					c_new -= 1;

					// Clip the triangle against a plane, based on the p variable. We only need to
					// test each subsequent plane against subsequent new triangles, as all
					// triangles after a plane clip are guaranteed to lie on the inside of the
					// plane.

					switch (p)
					{
						case 0:
						{
							c_clip = clip(vector(0.0, 1.0, 0.0), t_subject, c1, c2);

							break;
						}
						case 1:
						{
							c_clip = clip(vector(1.0, 0.0, 0.0), t_subject, c1, c2);

							break;
						}
						case 2:
						{
							t_subject.a.x = (double)width - t_subject.a.x;
							t_subject.b.x = (double)width - t_subject.b.x;
							t_subject.c.x = (double)width - t_subject.c.x;

							t_subject.a.y = (double)height - t_subject.a.y;
							t_subject.b.y = (double)height - t_subject.b.y;
							t_subject.c.y = (double)height - t_subject.c.y;

							c_clip = clip(vector(0.0, 1.0, 0.0), t_subject, c1, c2);

							c1.a.x = (double)width - c1.a.x;
							c1.b.x = (double)width - c1.b.x;
							c1.c.x = (double)width - c1.c.x;
							c2.a.x = (double)width - c2.a.x;
							c2.b.x = (double)width - c2.b.x;
							c2.c.x = (double)width - c2.c.x;

							c1.a.y = (double)height - c1.a.y;
							c1.b.y = (double)height - c1.b.y;
							c1.c.y = (double)height - c1.c.y;
							c2.a.y = (double)height - c2.a.y;
							c2.b.y = (double)height - c2.b.y;
							c2.c.y = (double)height - c2.c.y;

							break;
						}
						case 3:
						{
							t_subject.a.x = (double)width - t_subject.a.x;
							t_subject.b.x = (double)width - t_subject.b.x;
							t_subject.c.x = (double)width - t_subject.c.x;

							t_subject.a.y = (double)height - t_subject.a.y;
							t_subject.b.y = (double)height - t_subject.b.y;
							t_subject.c.y = (double)height - t_subject.c.y;

							c_clip = clip(vector(1.0, 0.0, 0.0), t_subject, c1, c2);

							c1.a.x = (double)width - c1.a.x;
							c1.b.x = (double)width - c1.b.x;
							c1.c.x = (double)width - c1.c.x;
							c2.a.x = (double)width - c2.a.x;
							c2.b.x = (double)width - c2.b.x;
							c2.c.x = (double)width - c2.c.x;

							c1.a.y = (double)height - c1.a.y;
							c1.b.y = (double)height - c1.b.y;
							c1.c.y = (double)height - c1.c.y;
							c2.a.y = (double)height - c2.a.y;
							c2.b.y = (double)height - c2.b.y;
							c2.c.y = (double)height - c2.c.y;

							break;
						}
					}

					// We will now add the results of the clipping to the back of the triangle
					// list. After p increments, these triangles will be clipped against the
					// other planes.

					for (int w = 0; w < c_clip; w++)
					{
						if (w == 0)
						{
							l_clipped.push_back(c1);
						}
						else if (w == 1)
						{
							l_clipped.push_back(c2);
						}
					}
				}

				c_new = l_clipped.size();
			}

			// Finally, the triangles have been transformed, scaled, culled, projected, sorted and
			// clipped, and thus they are ready to be rasterized.

			for (auto &p: l_clipped)
			{
				ftrianglergb
				(
					p.a.x,
					p.a.y,

					p.b.x,
					p.b.y,

					p.c.x,
					p.c.y,

					p.color
				);
			}
		}

		free((void*)obj);

		if (iteration % 60 == 0)
		{
			std::cout << my_object.faces.size() << " triangles." << std::endl;
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
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