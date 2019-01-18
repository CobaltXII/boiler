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