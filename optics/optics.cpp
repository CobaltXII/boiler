// Optics? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

// Use 'real' instead of float or double. This makes it easy to switch between
// floating-point precisions.

typedef float real;

#include "point.hpp"

#include "intersectable.hpp"

#include "reflective_segment.hpp"

#include "refractive_segment.hpp"

#include "strobe_filter_segment.hpp"

#include "subtractive_filter_segment.hpp"

#include "emitter.hpp"

#include "line.hpp"

// Boiler game class.

struct game: boiler
{	
	// The side length of a grid square.

	const real grid = 8.0f;

	// The intersectable objects in the scene.

	std::vector<intersectable*> scene_intersectable;

	// The emitters in the scene.

	std::vector<emitter*> scene_emitter;

	// The point being dragged.

	point* dragged = nullptr;

	// Check if the mouse is within range of a point.

	inline bool within_point(point p)
	{
		return (mouse_x - p.x) * (mouse_x - p.x) + (mouse_y - p.y) * (mouse_y - p.y) < 5 * 5;
	}

	// Check if the mouse is within range of a point. If so, drag the point if
	// mouse_l is true.

	inline bool within_point_drag(point& p)
	{
		if (within_point(p))
		{
			if (mouse_l && (dragged == nullptr || dragged == &p))
			{
				dragged = &p;

				p.x = round(real(mouse_x) / grid) * grid;
				p.y = round(real(mouse_y) / grid) * grid;
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	// Draw a reflective_segment*.

	void draw_reflective_segment(reflective_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
		circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));

		// Draw segment.

		linergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, rgb(255, 255, 255));

		// Draw normal.

		real nx = object->p1.x + object->dx / 2.0f;
		real ny = object->p1.y + object->dy / 2.0f;

		linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
	};

	// Draw a refractive_segment*.

	void draw_refractive_segment(refractive_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
		circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));

		// Draw segment.

		dashedlinergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, 1, 1, 0, rgb(255, 255, 255));

		// Draw normal.

		real nx = object->p1.x + object->dx / 2.0f;
		real ny = object->p1.y + object->dy / 2.0f;

		linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
	};

	// Draw a strobe_filter_segment*.

	void draw_strobe_filter_segment(strobe_filter_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
		circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));

		// Draw segment.

		dashedlinergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, 5, 5, SDL_GetTicks() / 100, rgb(255, 255, 255));

		// Draw normal.

		real nx = object->p1.x + object->dx / 2.0f;
		real ny = object->p1.y + object->dy / 2.0f;

		linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
	};

	// Draw a subtractive_filter_segment*.

	void draw_subtractive_filter_segment(subtractive_filter_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
		circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));

		// Draw segment.

		linergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, rgb(object->s_r, object->s_g, object->s_b));

		// Draw normal.

		real nx = object->p1.x + object->dx / 2.0f;
		real ny = object->p1.y + object->dy / 2.0f;

		linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
	};

	// Draw a emitter*.

	void draw_emitter(emitter* object)
	{	
		circlergb(object->p.x, object->p.y, within_point_drag(object->p) * 2 + 3, rgb(255, 255, 255));

		real rx = 0.0f - object->n.y; 
		real ry = 0.0f + object->n.x; 

		real tmx = object->p.x + object->n.x * emitter_length / 2.0f;
		real tmy = object->p.y + object->n.y * emitter_length / 2.0f;

		real bmx = object->p.x - object->n.x * emitter_length / 2.0f;
		real bmy = object->p.y - object->n.y * emitter_length / 2.0f;

		// Draw body.
		
		linergb(tmx - rx * emitter_width / 2.0f, tmy - ry * emitter_width / 2.0f, tmx + rx * emitter_width / 2.0f, tmy + ry * emitter_width / 2.0f, rgb(255, 255, 255));
		linergb(bmx - rx * emitter_width / 2.0f, bmy - ry * emitter_width / 2.0f, bmx + rx * emitter_width / 2.0f, bmy + ry * emitter_width / 2.0f, rgb(255, 255, 255));

		linergb(tmx - rx * emitter_width / 2.0f, tmy - ry * emitter_width / 2.0f, bmx - rx * emitter_width / 2.0f, bmy - ry * emitter_width / 2.0f, rgb(255, 255, 255));
		linergb(tmx + rx * emitter_width / 2.0f, tmy + ry * emitter_width / 2.0f, bmx + rx * emitter_width / 2.0f, bmy + ry * emitter_width / 2.0f, rgb(255, 255, 255));

		// Draw normal.

		object->a = point(bmx - object->n.x * emitter_normal, bmy - object->n.y * emitter_normal);

		linergb(bmx, bmy, object->a.x, object->a.y, rgb(0, 255, 0));

		circlergb(object->a.x, object->a.y, within_point_drag(object->a) * 2 + 3, rgb(0, 255, 0));

		// Check for dragging.

		if (dragged == &object->a)
		{
			// Normal anchor is being dragged.

			real dx = mouse_x - object->p.x;
			real dy = mouse_y - object->p.y;

			// Calculate new normal.

			point new_normal = normalize(point(-dx, -dy));

			// Assign new normal and anchor.

			object->n = new_normal;

			object->a = point(bmx - object->n.x * emitter_normal, bmy - object->n.y * emitter_normal);
		}
	}

	// Initialize the Boiler framework.

	void steam() override
	{
		// Seed the random number generator.

		srand(time(NULL));

		// Initialize the Boiler window.

		width = 960;

		height = 536;

		title = "Optics (using Boiler)";

		// Add demonstration intersectable objects.

		for (int i = 0; i < 20; i++)
		{
			real rx1 = width - 64.0f;
			real rx2 = width - 32.0f;

			real ry1 = round((i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f) / grid) * grid;
			real ry2 = round((i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f) / grid) * grid;

			if (i > 9)
			{
				scene_intersectable.push_back(new refractive_segment(point(rx1, ry1), point(rx2, ry2)));
			}
			else
			{
				scene_intersectable.push_back(new reflective_segment(point(rx1, ry1), point(rx2, ry2)));
			}
		}

		for (int i = 0; i < 20; i++)
		{
			real rx1 = width - 64.0f - 48.0f;
			real rx2 = width - 32.0f - 48.0f;

			real ry1 = round((i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f) / grid) * grid;
			real ry2 = round((i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f) / grid) * grid;

			if (i < 5)
			{
				scene_intersectable.push_back(new strobe_filter_segment(point(rx1, ry1), point(rx2, ry2)));
			}
			else if (i < 10)
			{
				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 255, 0, 0));
			}
			else if (i < 15)
			{
				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 0, 255, 0));
			}
			else if (i < 20)
			{
				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 0, 0, 255));
			}
		}

		// Add demonstration emitters.

		for (int i = 0; i < 20; i++)
		{
			unsigned int c = hsl_to_rgb(i * (360 / 20), 1.0f, 0.5f);

			scene_emitter.push_back(new emitter(point(64.0f, i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f), point(-1.0f, 0.0f), mgetr(c), mgetg(c), mgetb(c)));
		}
	}

	// Cast from an emitter.

	void cast_from_emitter
	(
		point p1, 
		point p2, 

		point n, 

		unsigned char cr, 
		unsigned char cg, 
		unsigned char cb,

		int depth = 0, 

		intersectable* ignore = nullptr
	)
	{
		if (depth >= 256)
		{
			return;
		}

		real p1x = p1.x;
		real p1y = p1.y;

		real p2x = p2.x;
		real p2y = p2.y;

		// Check for intersections against all intersectable objects.

		intersectable* intersected = nullptr;

		real distance_squared = INFINITY;

		point intersection_point;

		for (int j = 0; j < scene_intersectable.size(); j++)
		{
			intersectable* intersectable_object = scene_intersectable[j];

			if (intersectable_object == ignore)
			{
				continue;
			}

			if (intersectable_object->type == intersectable_reflective_segment)
			{
				reflective_segment* cast_object = (reflective_segment*)(intersectable_object);

				point i_p;

				if (line_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p1, cast_object->p2, i_p))
				{
					// Hit the object.

					real i_p_distance_squared = (i_p.x - p1x) * (i_p.x - p1x) + (i_p.y - p1y) * (i_p.y - p1y);

					if (i_p_distance_squared < distance_squared)
					{
						intersected = intersectable_object;

						distance_squared = i_p_distance_squared;

						intersection_point = i_p;
					}
				}
			}
			else if (intersectable_object->type == intersectable_refractive_segment)
			{
				refractive_segment* cast_object = (refractive_segment*)(intersectable_object);

				point i_p;

				if (line_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p1, cast_object->p2, i_p))
				{
					// Hit the object.

					real i_p_distance_squared = (i_p.x - p1x) * (i_p.x - p1x) + (i_p.y - p1y) * (i_p.y - p1y);

					if (i_p_distance_squared < distance_squared)
					{
						intersected = intersectable_object;

						distance_squared = i_p_distance_squared;

						intersection_point = i_p;
					}
				}
			}
			else if (intersectable_object->type == intersectable_strobe_filter_segment)
			{
				strobe_filter_segment* cast_object = (strobe_filter_segment*)(intersectable_object);

				point i_p;

				if (line_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p1, cast_object->p2, i_p))
				{
					// Hit the object.

					real i_p_distance_squared = (i_p.x - p1x) * (i_p.x - p1x) + (i_p.y - p1y) * (i_p.y - p1y);

					if (i_p_distance_squared < distance_squared)
					{
						intersected = intersectable_object;

						distance_squared = i_p_distance_squared;

						intersection_point = i_p;
					}
				}
			}
			else if (intersectable_object->type == intersectable_subtractive_filter_segment)
			{
				subtractive_filter_segment* cast_object = (subtractive_filter_segment*)(intersectable_object);

				point i_p;

				if (line_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p1, cast_object->p2, i_p))
				{
					// Hit the object.

					real i_p_distance_squared = (i_p.x - p1x) * (i_p.x - p1x) + (i_p.y - p1y) * (i_p.y - p1y);

					if (i_p_distance_squared < distance_squared)
					{
						intersected = intersectable_object;

						distance_squared = i_p_distance_squared;

						intersection_point = i_p;
					}
				}
			}
		}

		if (intersected != nullptr)
		{
			dashedlinergb(p1x, p1y, intersection_point.x, intersection_point.y, 32, 3, SDL_GetTicks() / 10, rgb(cr, cg, cb));

			if (intersected->type == intersectable_reflective_segment)
			{
				reflective_segment* cast_object = (reflective_segment*)(intersected);

				// Reflect off the intersected reflective segment and cast a 
				// new ray.

				real n_dx = n.x;
				real n_dy = n.y;

				// Found the original ray's normal. Now, reflect it off the 
				// surface normal of the intersected reflective segment.

				real two_dot_ni = 2.0f * (n_dx * cast_object->n.x + n_dy * cast_object->n.y);

				real rx = n_dx - two_dot_ni * cast_object->n.x;
				real ry = n_dy - two_dot_ni * cast_object->n.y;

				// Create and cast the ray.

				cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), point(rx, ry), cr, cg, cb, depth + 1, intersected);
			}
			else if (intersected->type == intersectable_refractive_segment)
			{
				refractive_segment* cast_object = (refractive_segment*)(intersected);

				// Do refraction.

				real n_dx = n.x;
				real n_dy = n.y;

				// Flip surface normals based on the side of the origin point.

				real s_nx = cast_object->n.x;
				real s_ny = cast_object->n.y;

				if ((p1.x - cast_object->p1.x) * (cast_object->p2.y - cast_object->p1.y) - (p1.y - cast_object->p1.y) * (cast_object->p2.x - cast_object->p1.x) > 0.0f)
				{
					s_nx = -s_nx;
					s_ny = -s_ny;
				}

				real eta = 1.64f;

				real i_dot_n = n_dx * s_nx + n_dy * s_ny;

				real k = 1.0f - eta * eta * (1.0f - i_dot_n * i_dot_n);

				if (k < 0.0f)
				{
					return;
				}
				else
				{
					real rx = eta * n_dx - (eta * i_dot_n + sqrtf(k)) * s_nx;
					real ry = eta * n_dy - (eta * i_dot_n + sqrtf(k)) * s_ny;

					// Create and cast the ray.

					cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), point(rx, ry), cr, cg, cb, depth + 1, intersected);
				}
			}
			else if (intersected->type == intersectable_strobe_filter_segment)
			{
				strobe_filter_segment* cast_object = (strobe_filter_segment*)(intersected);

				int period = 500;

				if (SDL_GetTicks() % (period * 2) >= period)
				{
					cast_from_emitter(intersection_point, point(intersection_point.x + n.x * 1024.0f, intersection_point.y + n.y * 1024.0f), n, cr, cg, cb, depth + 1, intersected);
				}
			}
			else if (intersected->type == intersectable_subtractive_filter_segment)
			{
				subtractive_filter_segment* cast_object = (subtractive_filter_segment*)(intersected);

				cast_from_emitter
				(
					intersection_point, 

					point
					(
						intersection_point.x + n.x * 1024.0f, 
						intersection_point.y + n.y * 1024.0f
					), 

					n, 

					(int(cr) - int(cast_object->s_r)) < 0 ? 0 : (int(cr) - int(cast_object->s_r)), 
					(int(cg) - int(cast_object->s_g)) < 0 ? 0 : (int(cg) - int(cast_object->s_g)), 
					(int(cb) - int(cast_object->s_b)) < 0 ? 0 : (int(cb) - int(cast_object->s_b)), 

					depth + 1, 

					intersected
				);
			}
		}
		else
		{
			dashedlinergb(p1x, p1y, p2x, p2y, 32, 3, SDL_GetTicks() / 10, rgb(cr, cg, cb));
		}
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