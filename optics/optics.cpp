// Optics? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include "../dos/dos_gui.h"

#include <climits>
#include <cmath>
#include <ctime>

#include <vector>
#include <utility>
#include <sstream>
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

#include "reflective_circle.hpp"

#include "refractive_circle.hpp"

#include "emitter.hpp"

#include "line.hpp"

// Menu item callback prototype.

int menu_item_callback(void* userdata, std::string label, int tab, int index);

// Game states.

enum game_state
{
	gs_default, gs_dialog, gs_place
};

// Boiler game class.

struct game: boiler
{	
	// The GUI renderer.

	dos_gui GUI = dos_gui(104, 35, this, menu_item_callback, this);

	// The dialog strings.

	std::string dialog_title;
	
	std::string dialog_caption;

	std::string dialog_text;

	// The game state.

	game_state state = gs_default;

	bool normals = true;

	bool editing = true;

	int delete_selection = 0;

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
		if (within_point(p) && state != gs_dialog && editing)
		{
			if (mouse_l && (dragged == nullptr || dragged == &p))
			{
				dragged = &p;

				p.x = round(real(mouse_x) / grid) * grid;
				p.y = round(real(mouse_y) / grid) * grid;

				if (GUI.status_text == "Click on the point to center on the X axis")
				{
					p.x = round(real(width / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}
				else if (GUI.status_text == "Click on the point to center on the Y axis")
				{
					p.y = round(real(height / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}
				else if (GUI.status_text == "Click on the point to center on both axes")
				{
					p.x = round(real(width / 2) / grid) * grid;

					p.y = round(real(height / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	// Return 0 if nothing, 1 if inside mouse, and 2 if inside mouse and mouse 
	// down.

	inline int int_within_point_drag(point& p)
	{
		if (within_point(p) && state != gs_dialog && editing)
		{
			if (mouse_l && (dragged == nullptr || dragged == &p))
			{
				dragged = &p;

				p.x = round(real(mouse_x) / grid) * grid;
				p.y = round(real(mouse_y) / grid) * grid;

				if (GUI.status_text == "Click on the point to center on the X axis")
				{
					p.x = round(real(width / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}
				else if (GUI.status_text == "Click on the point to center on the Y axis")
				{
					p.y = round(real(height / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}
				else if (GUI.status_text == "Click on the point to center on both axes")
				{
					p.x = round(real(width / 2) / grid) * grid;

					p.y = round(real(height / 2) / grid) * grid;

					dragged = nullptr;

					state = gs_default;

					GUI.locked_menus = false;
				}

				return 2;
			}

			return 1;
		}
		else
		{
			return 0;
		}
	}

	// Draw a reflective_segment*.

	void draw_reflective_segment(reflective_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		if (editing)
		{
			circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
			circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));
		}

		// Draw segment.

		linergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, rgb(255, 255, 255));

		// Draw normal.

		if (normals && editing)
		{
			real nx = object->p1.x + object->dx / 2.0f;
			real ny = object->p1.y + object->dy / 2.0f;

			linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
		}
	};

	// Draw a refractive_segment*.

	void draw_refractive_segment(refractive_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		if (editing)
		{
			circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
			circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));
		}

		// Draw segment.

		dashedlinergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, 1, 1, 0, rgb(255, 255, 255));

		// Draw normal.

		if (normals && editing)
		{
			real nx = object->p1.x + object->dx / 2.0f;
			real ny = object->p1.y + object->dy / 2.0f;

			linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
		}
	};

	// Draw a strobe_filter_segment*.

	void draw_strobe_filter_segment(strobe_filter_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		if (editing)
		{
			circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
			circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));
		}

		// Draw segment.

		dashedlinergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, 5, 5, SDL_GetTicks() / 100, rgb(255, 255, 255));

		// Draw normal.

		if (normals && editing)
		{
			real nx = object->p1.x + object->dx / 2.0f;
			real ny = object->p1.y + object->dy / 2.0f;

			linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
		}
	};

	// Draw a subtractive_filter_segment*.

	void draw_subtractive_filter_segment(subtractive_filter_segment* object)
	{
		const real normal_length = 16.0f;

		// Draw and drag endpoints.

		if (editing)
		{
			circlergb(object->p1.x, object->p1.y, within_point_drag(object->p1) * 2 + 3, rgb(255, 255, 255));
			circlergb(object->p2.x, object->p2.y, within_point_drag(object->p2) * 2 + 3, rgb(255, 255, 255));
		}

		// Draw segment.

		linergb(object->p1.x, object->p1.y, object->p2.x, object->p2.y, rgb(object->s_r, object->s_g, object->s_b));

		// Draw normal.

		if (normals && editing)
		{
			real nx = object->p1.x + object->dx / 2.0f;
			real ny = object->p1.y + object->dy / 2.0f;

			linergb(nx, ny, nx + object->n.x * normal_length, ny + object->n.y * normal_length, rgb(0, 255, 0));
		}
	};

	// Draw a reflective_circle*.

	void draw_reflective_circle(reflective_circle* object)
	{
		// Draw the circle.

		circlergb(object->p.x, object->p.y, object->r, rgb(255, 255, 255));

		// Draw the drag point.

		if (editing)
		{
			circlergb(object->p.x, object->p.y, within_point_drag(object->p) * 2 + 3, rgb(255, 255, 255));
		}

		if (dragged == &(object->p))
		{
			object->a.x = object->p.x + object->r;
		}

		object->a.y = object->p.y;

		if (editing)
		{
			// Draw the radius marker.

			linergb(object->p.x, object->p.y, object->a.x, object->a.y, rgb(0, 255, 0));

			// Draw the radius anchor.

			circlergb(object->a.x, object->a.y, within_point_drag(object->a) * 2 + 3, rgb(0, 255, 0));
		}
	};

	// Draw a refractive_circle*.

	void draw_refractive_circle(refractive_circle* object)
	{
		// Draw the circle.

		dashedcirclergb(object->p.x, object->p.y, object->r, 5, 5, SDL_GetTicks() / 100, rgb(255, 255, 255));

		// Draw the drag point.

		if (editing)
		{
			circlergb(object->p.x, object->p.y, within_point_drag(object->p) * 2 + 3, rgb(255, 255, 255));
		}

		if (dragged == &(object->p))
		{
			object->a.x = object->p.x + object->r;
		}

		object->a.y = object->p.y;

		if (editing)
		{
			// Draw the radius marker.

			linergb(object->p.x, object->p.y, object->a.x, object->a.y, rgb(0, 255, 0));

			// Draw the radius anchor.

			circlergb(object->a.x, object->a.y, within_point_drag(object->a) * 2 + 3, rgb(0, 255, 0));
		}
	};

	// Draw an intersectable*.

	void draw_intersectable(intersectable* object)
	{
		if ((*object).type == intersectable_reflective_segment)
		{
			reflective_segment* cast_object = (reflective_segment*)(object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}

			draw_reflective_segment(cast_object);
		}
		else if ((*object).type == intersectable_refractive_segment)
		{
			refractive_segment* cast_object = (refractive_segment*)(object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}

			draw_refractive_segment(cast_object);
		}
		else if ((*object).type == intersectable_strobe_filter_segment)
		{
			strobe_filter_segment* cast_object = (strobe_filter_segment*)(object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}

			draw_strobe_filter_segment(cast_object);
		}
		else if ((*object).type == intersectable_subtractive_filter_segment)
		{
			subtractive_filter_segment* cast_object = (subtractive_filter_segment*)(object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}

			draw_subtractive_filter_segment(cast_object);
		}
		else if ((*object).type == intersectable_reflective_circle)
		{
			reflective_circle* cast_object = (reflective_circle*)(object);

			draw_reflective_circle(cast_object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}
		}
		else if ((*object).type == intersectable_refractive_circle)
		{
			refractive_circle* cast_object = (refractive_circle*)(object);

			draw_refractive_circle(cast_object);

			if (cast_object->changed())
			{
				cast_object->recalculate();
			}
		}
	}

	// Draw a emitter*.

	void draw_emitter(emitter* object)
	{	
		int touch = int_within_point_drag(object->p);

		if (touch == 2 && GUI.status_text == "Click on the Emitter to enable")
		{
			object->state = true;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to disable")
		{
			object->state = false;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to enable Auto Spin on")
		{
			object->auto_spin = true;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to enable Auto Cycle on")
		{
			object->auto_cycle = true;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to disable Auto Spin on")
		{
			object->auto_spin = false;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to disable Auto Cycle on")
		{
			object->auto_cycle = false;

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to Face North")
		{
			object->n = point(0.0f, -1.0f);

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to Face East")
		{
			object->n = point(1.0f, 0.0f);
			
			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to Face South")
		{
			object->n = point(0.0f, 1.0f);
			
			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (touch == 2 && GUI.status_text == "Click on the Emitter to Face West")
		{
			object->n = point(-1.0f, 0.0f);
			
			state = gs_default;

			GUI.locked_menus = false;
		}

		if (editing)
		{
			circlergb(object->p.x, object->p.y, std::min(1, touch) * 2 + 3, rgb(255, 255, 255));
		}

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

		if (editing)
		{
			object->a = point(bmx - object->n.x * emitter_normal, bmy - object->n.y * emitter_normal);

			linergb(bmx, bmy, object->a.x, object->a.y, rgb(0, 255, 0));

			circlergb(object->a.x, object->a.y, within_point_drag(object->a) * 2 + 3, rgb(0, 255, 0));
		}

		// Check for dragging.

		if (dragged == &object->a)
		{
			// Normal anchor is being dragged.

			real dx = mouse_x - object->p.x;
			real dy = mouse_y - object->p.y;

			// Calculate new normal.

			point new_normal = normalize(point(-dx, -dy));

			// Assign new normal and anchor.

			object->n = normalize(new_normal);

			object->a = point(bmx - object->n.x * emitter_normal, bmy - object->n.y * emitter_normal);
		}
	}

	// Initialize the Boiler framework.

	void steam() override
	{
		// Seed the random number generator.

		srand(time(NULL));

		// Initialize the GUI.

		GUI.main_menu =
		{
			"Emitter", "Segment", "Filter", "Circle", "Center", "Edit", "Settings", "Help"
		};

		GUI.main_menu_contents =
		{
			{
				"Add White Emitter", "Add Colored Emitter", "", "Enable Emitter", "Disable Emitter", "", "Enable Auto Spin", "Enable Auto Cycle", "Disable Auto Spin", "Disable Auto Cycle", "", "Face North", "Face East", "Face South", "Face West"
			},

			{
				"Add Reflective Segment", "Add Refractive Segment"
			},

			{
				"Add Strobe Filter", "Add Red Subtractive Filter", "Add Green Subtractive Filter", "Add Blue Subtractive Filter"
			},

			{
				"Add Reflective Circle", "Add Refractive Circle"
			},

			{
				"Center On X Axis", "Center On Y Axis", "Center On Both Axes"
			},

			{
				"Delete Intersectable", "Delete Emitter", "", "Delete All Intersectables", "Delete All Emitters"
			},

			{
				"Enable Normals", "Disable Normals", "", "Enable Editing", "Disable Editing"
			},

			{
				"About"
			}
		};

		GUI.finalize();

		// Initialize the Boiler window.

		width = GUI.x_res;

		height = GUI.y_res;

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

			scene_emitter.push_back(new emitter(point(64.0f, round((i * 24.0f + (height - (19.0f * 24.0f)) / 2.0f) / grid) * grid), point(-1.0f, 0.0f), mgetr(c), mgetg(c), mgetb(c)));
		}

		// Add white emitter.

		scene_emitter.push_back(new emitter(point(round(width / 2.0f / grid) * grid + grid, round(height / 2.0f / grid) * grid), point(0.0f, -1.0f), 255, 255, 255));

		// Add circles.

		scene_intersectable.push_back(new refractive_circle(point(round(width / 2.0f / grid) * grid, round(height / 2.0f / grid) * grid), 32.0f));

		scene_intersectable.push_back(new reflective_circle(point(round(width / 2.0f / grid) * grid, round(height / 2.0f / grid) * grid), 64.0f));
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
			else if (intersectable_object->type == intersectable_reflective_circle)
			{
				reflective_circle* cast_object = (reflective_circle*)(intersectable_object);

				point intersection1;
				point intersection2;

				int intersect = circle_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p, cast_object->r, intersection1, intersection2);

				if (intersect)
				{
					// Hit the object.

					if (intersect == 1)
					{
						real i_p_distance_squared = (intersection1.x - p1x) * (intersection1.x - p1x) + (intersection1.y - p1y) * (intersection1.y - p1y);

						if (i_p_distance_squared < distance_squared)
						{
							intersected = intersectable_object;

							distance_squared = i_p_distance_squared;

							intersection_point = intersection1;
						}
					}
					else
					{
						real i1_distance_squared = (intersection1.x - p1x) * (intersection1.x - p1x) + (intersection1.y - p1y) * (intersection1.y - p1y);
						real i2_distance_squared = (intersection2.x - p1x) * (intersection2.x - p1x) + (intersection2.y - p1y) * (intersection2.y - p1y);

						// Use the closest intersection.

						if (i1_distance_squared < i2_distance_squared)
						{
							if (i1_distance_squared < distance_squared)
							{
								intersected = intersectable_object;

								distance_squared = i1_distance_squared;

								intersection_point = intersection1;
							}
						}
						else
						{
							if (i2_distance_squared < distance_squared)
							{
								intersected = intersectable_object;

								distance_squared = i2_distance_squared;

								intersection_point = intersection2;
							}
						}
					}
				}
			}
			else if (intersectable_object->type == intersectable_refractive_circle)
			{
				refractive_circle* cast_object = (refractive_circle*)(intersectable_object);

				point intersection1;
				point intersection2;

				int intersect = circle_intersect(point(p1x, p1y), point(p2x, p2y), cast_object->p, cast_object->r, intersection1, intersection2);

				if (intersect)
				{
					// Hit the object.

					if (intersect == 1)
					{
						real i_p_distance_squared = (intersection1.x - p1x) * (intersection1.x - p1x) + (intersection1.y - p1y) * (intersection1.y - p1y);

						if (i_p_distance_squared < distance_squared)
						{
							intersected = intersectable_object;

							distance_squared = i_p_distance_squared;

							intersection_point = intersection1;
						}
					}
					else
					{
						real i1_distance_squared = (intersection1.x - p1x) * (intersection1.x - p1x) + (intersection1.y - p1y) * (intersection1.y - p1y);
						real i2_distance_squared = (intersection2.x - p1x) * (intersection2.x - p1x) + (intersection2.y - p1y) * (intersection2.y - p1y);

						// Use the closest intersection.

						if (i1_distance_squared < i2_distance_squared)
						{
							if (i1_distance_squared < distance_squared)
							{
								intersected = intersectable_object;

								distance_squared = i1_distance_squared;

								intersection_point = intersection1;
							}
						}
						else
						{
							if (i2_distance_squared < distance_squared)
							{
								intersected = intersectable_object;

								distance_squared = i2_distance_squared;

								intersection_point = intersection2;
							}
						}
					}
				}
			}
		}

		if (intersected != nullptr)
		{
			linergb(p1x, p1y, intersection_point.x, intersection_point.y, rgb(cr, cg, cb));

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

				cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), cr, cg, cb, depth + 1, intersected);
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

				real eta = 1.32f;

				real i_dot_n = n_dx * s_nx + n_dy * s_ny;

				real k = 1.0f - eta * eta * (1.0f - i_dot_n * i_dot_n);

				if (k < 0.0f)
				{
					return;
				}
				else
				{
					if (cr == 255 && cg == 255 && cb == 255)
					{
						// It's white, split into rainbow.

						const int bands = 64;

						for (int b = 0; b < bands; b++)
						{
							real diff = ((real(b) / real(bands)) * 2.0f - 1.0f) * 0.1f;

							unsigned int split_color = hsl_to_rgb(b * (360 / bands), 1.0f, 0.5f);

							unsigned char br = mgetr(split_color);
							unsigned char bg = mgetg(split_color);
							unsigned char bb = mgetb(split_color);

							real rx = (eta + diff) * n_dx - (eta * i_dot_n + sqrtf(k)) * s_nx;
							real ry = (eta + diff) * n_dy - (eta * i_dot_n + sqrtf(k)) * s_ny;

							// Create and cast the ray.

							cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), br, bg, bb, depth + 1, intersected);
						}
					}
					else
					{
						real rx = eta * n_dx - (eta * i_dot_n + sqrtf(k)) * s_nx;
						real ry = eta * n_dy - (eta * i_dot_n + sqrtf(k)) * s_ny;

						// Create and cast the ray.

						cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), cr, cg, cb, depth + 1, intersected);
					}
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
			else if (intersected->type == intersectable_reflective_circle)
			{
				reflective_circle* cast_object = (reflective_circle*)(intersected);

				real nx = (intersection_point.x - cast_object->p.x) / cast_object->r;
				real ny = (intersection_point.y - cast_object->p.y) / cast_object->r;

				real two_dot_ni = 2.0f * (n.x * nx + n.y * ny);

				real rx = n.x - two_dot_ni * nx;
				real ry = n.y - two_dot_ni * ny;

				cast_from_emitter(point(intersection_point.x + rx * 1.0f, intersection_point.y + ry * 1.0f), point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), cr, cg, cb, depth + 1, nullptr);
			}
			else if (intersected->type == intersectable_refractive_circle)
			{
				refractive_circle* cast_object = (refractive_circle*)(intersected);

				// Do refraction.

				real n_dx = n.x;
				real n_dy = n.y;

				// Circle normal.

				real s_nx = (intersection_point.x - cast_object->p.x) / cast_object->r;
				real s_ny = (intersection_point.y - cast_object->p.y) / cast_object->r;

				real eta = 1.32f;

				real i_dot_n = n_dx * s_nx + n_dy * s_ny;

				real k = 1.0f - eta * eta * (1.0f - i_dot_n * i_dot_n);

				if (k < 0.0f)
				{
					return;
				}
				else
				{
					if (cr == 255 && cg == 255 && cb == 255)
					{
						// It's white, split into rainbow.

						const int bands = 64;

						for (int b = 0; b < bands; b++)
						{
							real diff = ((real(b) / real(bands)) * 2.0f - 1.0f) * 0.1f;

							unsigned int split_color = hsl_to_rgb(b * (360 / bands), 1.0f, 0.5f);

							unsigned char br = mgetr(split_color);
							unsigned char bg = mgetg(split_color);
							unsigned char bb = mgetb(split_color);

							real rx = (eta + diff) * n_dx - (eta * i_dot_n + sqrtf(k)) * s_nx;
							real ry = (eta + diff) * n_dy - (eta * i_dot_n + sqrtf(k)) * s_ny;

							// Create and cast the ray.

							cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), br, bg, bb, depth + 1, intersected);
						}
					}
					else
					{
						real rx = eta * n_dx - (eta * i_dot_n + sqrtf(k)) * s_nx;
						real ry = eta * n_dy - (eta * i_dot_n + sqrtf(k)) * s_ny;

						// Create and cast the ray.

						cast_from_emitter(intersection_point, point(intersection_point.x + rx * 1024.0f, intersection_point.y + ry * 1024.0f), normalize(point(rx, ry)), cr, cg, cb, depth + 1, intersected);
					}
				}
			}
		}
		else
		{
			linergb(p1x, p1y, p2x, p2y, rgb(cr, cg, cb));
		}
	}

	// Handle keypresses.

	void keydown(SDL_Event e) override
	{
		SDL_Keycode key = e.key.keysym.sym;

		if (key == SDLK_ESCAPE)
		{
			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (key == SDLK_SPACE)
		{
			if (GUI.status_text == "Press SPACE to drop a White Emitter")
			{
				scene_emitter.push_back(new emitter(point(round(mouse_x / grid) * grid, round(mouse_y / grid) * grid), point(0.0f, -1.0f), 255, 255, 255));
			}
			else if (GUI.status_text == "Press SPACE to drop a Colored Emitter")
			{
				unsigned int c = hsl_to_rgb(rand() % 360, 1.0f, 0.5);

				unsigned char cr = mgetr(c);
				unsigned char cg = mgetg(c);
				unsigned char cb = mgetb(c);

				scene_emitter.push_back(new emitter(point(round(mouse_x / grid) * grid, round(mouse_y / grid) * grid), point(0.0f, -1.0f), cr, cg, cb));
			}
			else if (GUI.status_text == "Press SPACE to drop a Reflective Circle")
			{
				scene_intersectable.push_back(new reflective_circle(point(round(mouse_x / grid) * grid, round(mouse_y / grid) * grid), 48.0f));
			}
			else if (GUI.status_text == "Press SPACE to drop a Refractive Circle")
			{
				scene_intersectable.push_back(new refractive_circle(point(round(mouse_x / grid) * grid, round(mouse_y / grid) * grid), 48.0f));
			}
			else if (GUI.status_text == "Press SPACE to drop a Reflective Segment")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new reflective_segment(point(rx1, ry1), point(rx2, ry2)));
			}
			else if (GUI.status_text == "Press SPACE to drop a Refractive Segment")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new refractive_segment(point(rx1, ry1), point(rx2, ry2)));
			}
			else if (GUI.status_text == "Press SPACE to drop a Strobe Filter")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new strobe_filter_segment(point(rx1, ry1), point(rx2, ry2)));
			}
			else if (GUI.status_text == "Press SPACE to drop a Red Subtractive Filter")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 255, 0, 0));
			}
			else if (GUI.status_text == "Press SPACE to drop a Green Subtractive Filter")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 0, 255, 0));
			}
			else if (GUI.status_text == "Press SPACE to drop a Blue Subtractive Filter")
			{
				real rx1 = round(mouse_x / grid) * grid - (grid * 3);
				real rx2 = round(mouse_x / grid) * grid + (grid * 3);

				real ry1 = round(mouse_y / grid) * grid;
				real ry2 = round(mouse_y / grid) * grid;

				scene_intersectable.push_back(new subtractive_filter_segment(point(rx1, ry1), point(rx2, ry2), 0, 0, 255));
			}

			state = gs_default;

			GUI.locked_menus = false;
		}
		else if (key == SDLK_LEFT)
		{
			delete_selection--;
		}
		else if (key == SDLK_RIGHT)
		{
			delete_selection++;
		}
		else if (key == SDLK_RETURN)
		{
			if (GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Intersectable.")
			{
				scene_intersectable.erase(scene_intersectable.begin() + delete_selection);

				state = gs_default;

				GUI.locked_menus = false;
			}
			else if (GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Emitter.")
			{
				scene_emitter.erase(scene_emitter.begin() + delete_selection);

				state = gs_default;

				GUI.locked_menus = false;
			}
		}
	}

	// Draw a frame.

	void draw() override
	{
		clear(vga_rgb[vga_dark_blue]);

		if 
		(
			GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Intersectable." ||

			GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Emitter."
		)
		{
			goto done_render;
		}

		// Do emitter casting.

		for (int i = 0; i < scene_emitter.size(); i++)
		{
			emitter* object = scene_emitter[i];

			if (object->auto_spin)
			{
				object->n = point
				(
					cos(SDL_GetTicks() / 1000.0f + i * 13.0f),
					sin(SDL_GetTicks() / 1000.0f + i * 13.0f)
				);
			}

			if (!object->state)
			{
				// Emitter is off.

				continue;
			}

			real tmx = object->p.x + object->n.x * emitter_length / 2.0f;
			real tmy = object->p.y + object->n.y * emitter_length / 2.0f;

			real p1x = tmx;
			real p1y = tmy;

			real p2x = tmx + object->n.x * 1024.0f;
			real p2y = tmy + object->n.y * 1024.0f;

			if (object->auto_cycle)
			{
				unsigned int c = hsl_to_rgb((SDL_GetTicks() / 25) % 360, 1.0f, 0.5f);

				cast_from_emitter
				(
					point(p1x, p1y), 
					point(p2x, p2y), 

					normalize(object->n),

					mgetr(c),
					mgetg(c),
					mgetb(c)
				);
			}
			else
			{
				cast_from_emitter
				(
					point(p1x, p1y), 
					point(p2x, p2y), 

					normalize(object->n),

					object->cr,
					object->cg,
					object->cb
				);
			}
		}

		// Draw the intersectable objects in the scene.

		for (int i = 0; i < scene_intersectable.size(); i++)
		{
			draw_intersectable(scene_intersectable[i]);
		}

		// Draw the emitter objects in the scene.

		for (int i = 0; i < scene_emitter.size(); i++)
		{
			emitter* object = scene_emitter[i];

			draw_emitter(object);
		}

		// Continue dragging.

		if (dragged != nullptr && mouse_l)
		{
			dragged->x = round(real(mouse_x) / grid) * grid;
			dragged->y = round(real(mouse_y) / grid) * grid;
		}

		if (!mouse_l)
		{
			dragged = nullptr;
		}

		done_render:

		if (GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Intersectable.")
		{
			if (SDL_GetTicks() % 200 >= 100 - 1)
			{
				delete_selection = std::max(0, std::min(delete_selection, int(scene_intersectable.size() - 1)));

				draw_intersectable(scene_intersectable[delete_selection]);
			}
			
			for (int i = 0; i < scene_intersectable.size(); i++)
			{
				if (i != delete_selection)
				{
					draw_intersectable(scene_intersectable[i]);
				}
			}

			for (int i = 0; i < scene_emitter.size(); i++)
			{
				emitter* object = scene_emitter[i];

				draw_emitter(object);
			}
		}
		else if (GUI.status_text == "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Emitter.")
		{
			if (SDL_GetTicks() % 200 >= 100 - 1)
			{
				delete_selection = std::max(0, std::min(delete_selection, int(scene_emitter.size() - 1)));

				draw_emitter(scene_emitter[delete_selection]);
			}
			
			for (int i = 0; i < scene_emitter.size(); i++)
			{
				if (i != delete_selection)
				{
					draw_emitter(scene_emitter[i]);
				}
			}

			for (int i = 0; i < scene_intersectable.size(); i++)
			{
				draw_intersectable(scene_intersectable[i]);
			}
		}

		// Generate the status text.

		if (state == gs_default)
		{
			std::stringstream status_text;

			status_text << round(real(mouse_x) / grid) << ", " << round(real(mouse_y) / grid) << " (" << mouse_x << ", " << mouse_y << ")";

			status_text << " | " << scene_intersectable.size() << " intersectables, " << scene_emitter.size() << " emitters";

			GUI.status_text = status_text.str();
		}

		// Render the GUI.

		GUI.render();

		if (state == gs_dialog)
		{
			GUI.draw_dialog(dialog_title, dialog_caption, dialog_text);
		}

		GUI.display();
	}
};

// Menu item callback.

int menu_item_callback(void* userdata, std::string label, int tab, int index)
{
	game* parent = (game*)userdata;

	if (label == "Add White Emitter")
	{
		parent->GUI.status_text = "Press SPACE to drop a White Emitter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Colored Emitter")
	{
		parent->GUI.status_text = "Press SPACE to drop a Colored Emitter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Enable Emitter")
	{
		parent->GUI.status_text = "Click on the Emitter to enable";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Disable Emitter")
	{
		parent->GUI.status_text = "Click on the Emitter to disable";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Enable Auto Spin")
	{
		parent->GUI.status_text = "Click on the Emitter to enable Auto Spin on";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Enable Auto Cycle")
	{
		parent->GUI.status_text = "Click on the Emitter to enable Auto Cycle on";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Disable Auto Spin")
	{
		parent->GUI.status_text = "Click on the Emitter to disable Auto Spin on";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Disable Auto Cycle")
	{
		parent->GUI.status_text = "Click on the Emitter to disable Auto Cycle on";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Face North")
	{
		parent->GUI.status_text = "Click on the Emitter to Face North";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Face East")
	{
		parent->GUI.status_text = "Click on the Emitter to Face East";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Face South")
	{
		parent->GUI.status_text = "Click on the Emitter to Face South";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Face West")
	{
		parent->GUI.status_text = "Click on the Emitter to Face West";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "About")
	{
		parent->GUI.status_text = "Press ESC to escape dialog";

		parent->state = gs_dialog;

		parent->dialog_title = "About Optics";

		parent->dialog_caption = "About Optics";

		std::stringstream dialog_text;

		dialog_text << "The program 'Optics' was written by the developer Co";
		dialog_text << "baltXII (Adam Sidat) on January 15, 2019. The projec";
		dialog_text << "t gained ideas from users of the OneLoneCoder Discor";
		dialog_text << "d server. Thank you to all of the original beta test";
		dialog_text << "ers, supporters and idea providers.                 ";

		parent->dialog_text = dialog_text.str();

		parent->state = gs_dialog;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "Add Reflective Segment")
	{
		parent->GUI.status_text = "Press SPACE to drop a Reflective Segment";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Refractive Segment")
	{
		parent->GUI.status_text = "Press SPACE to drop a Refractive Segment";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "Add Strobe Filter")
	{
		parent->GUI.status_text = "Press SPACE to drop a Strobe Filter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Red Subtractive Filter")
	{
		parent->GUI.status_text = "Press SPACE to drop a Red Subtractive Filter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Green Subtractive Filter")
	{
		parent->GUI.status_text = "Press SPACE to drop a Green Subtractive Filter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Blue Subtractive Filter")
	{
		parent->GUI.status_text = "Press SPACE to drop a Blue Subtractive Filter";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "Add Reflective Circle")
	{
		parent->GUI.status_text = "Press SPACE to drop a Reflective Circle";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Add Refractive Circle")
	{
		parent->GUI.status_text = "Press SPACE to drop a Refractive Circle";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "Center On X Axis")
	{
		parent->GUI.status_text = "Click on the point to center on the X axis";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Center On Y Axis")
	{
		parent->GUI.status_text = "Click on the point to center on the Y axis";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Center On Both Axes")
	{
		parent->GUI.status_text = "Click on the point to center on both axes";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}

	if (label == "Delete Intersectable")
	{
		parent->GUI.status_text = "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Intersectable.";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Delete Emitter")
	{
		parent->GUI.status_text = "Use LEFT and RIGHT to switch between objects. Press RETURN to delete the selected Emitter.";

		parent->state = gs_place;

		return DOS_MIR_HIDE_LOCK;
	}
	else if (label == "Delete All Intersectables")
	{
		parent->scene_intersectable.clear();
	}
	else if (label == "Delete All Emitters")
	{
		parent->scene_emitter.clear();
	}

	if (label == "Enable Normals")
	{
		parent->normals = true;
	}
	else if (label == "Disable Normals")
	{
		parent->normals = false;
	}
	else if (label == "Enable Editing")
	{
		parent->editing = true;
	}
	else if (label == "Disable Editing")
	{
		parent->editing = false;
	}

	return DOS_MIR_DEFAULT;
}

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