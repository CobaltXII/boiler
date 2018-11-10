// Bezier curves? Awesome.

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

struct point
{
	double x;
	double y;

	point(double _x = 0.0, double _y = 0.0)
	{
		x = _x;
		y = _y;
	}
};

struct game: boiler
{	
	std::vector<point> points;

	point* selected = NULL;

	bool debug = true;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Bezier curves (using Boiler)";

		f_No_Debug = SDL_TRUE;

		// Initialize point array.

		points.push_back(point(100, 100));

		points.push_back(point(100, height - 100));

		points.push_back(point(width - 100, 100));

		points.push_back(point(width - 100, height - 100));
	}

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_SPACE)
		{
			debug = !debug;
		}
	}

	void draw() override
	{
		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Make points movable.

		if (mouse_l == true && selected == NULL)
		{
			for (int i = 0; i < points.size(); i++)
			{
				double dx = mouse_x - points[i].x;
				double dy = mouse_y - points[i].y;

				if (dx * dx + dy * dy <= 7 * 7)
				{
					selected = &(points[i]);

					break;
				}
			}
		}
		else if (mouse_l == true && selected)
		{
			selected->x = mouse_x;
			selected->y = mouse_y;
		}
		else
		{
			selected = NULL;
		}

		// Draw points.

		for (int i = 0; i < points.size(); i++)
		{
			circlergb(points[i].x, points[i].y, 7, rgb(255, 255, 255));
		}

		// Draw lines.

		linergb(points[0].x, points[0].y, points[1].x, points[1].y, rgb(255, 0, 0));
		linergb(points[1].x, points[1].y, points[2].x, points[2].y, rgb(255, 0, 0));
		linergb(points[2].x, points[2].y, points[3].x, points[3].y, rgb(255, 0, 0));

		// Draw the two quadratic Bezier curves.

		int im = 1000;

		if (debug)
		{
			for (int i = 0; i < im; i++)
			{
				double t = (double)i / (double)im;

				double b01x = (1.0 - t) * points[0].x + t * points[1].x;
				double b01y = (1.0 - t) * points[0].y + t * points[1].y;

				double b12x = (1.0 - t) * points[1].x + t * points[2].x;
				double b12y = (1.0 - t) * points[1].y + t * points[2].y;

				double b23x = (1.0 - t) * points[2].x + t * points[3].x;
				double b23y = (1.0 - t) * points[2].y + t * points[3].y;

				double b012x = (1.0 - t) * b01x + t * b12x;
				double b012y = (1.0 - t) * b01y + t * b12y;

				double b123x = (1.0 - t) * b12x + t * b23x;
				double b123y = (1.0 - t) * b12y + t * b23y;

				if (b012x >= 0 && b012y >= 0 && b012x < width && b012y < height)
				{
					pixels[int(b012y) * width + int(b012x)] = rgb(255, 0, 255);
				}

				if (b123x >= 0 && b123y >= 0 && b123x < width && b123y < height)
				{
					pixels[int(b123y) * width + int(b123x)] = rgb(255, 0, 255);
				}
			}
		}

		// Draw the cubic Bezier curve.

		for (int i = 0; i < im; i++)
		{
			double t = (double)i / (double)im;

			double b01x = (1.0 - t) * points[0].x + t * points[1].x;
			double b01y = (1.0 - t) * points[0].y + t * points[1].y;

			double b12x = (1.0 - t) * points[1].x + t * points[2].x;
			double b12y = (1.0 - t) * points[1].y + t * points[2].y;

			double b23x = (1.0 - t) * points[2].x + t * points[3].x;
			double b23y = (1.0 - t) * points[2].y + t * points[3].y;

			double b012x = (1.0 - t) * b01x + t * b12x;
			double b012y = (1.0 - t) * b01y + t * b12y;

			double b123x = (1.0 - t) * b12x + t * b23x;
			double b123y = (1.0 - t) * b12y + t * b23y;

			double b0123x = (1.0 - t) * b012x + t * b123x;
			double b0123y = (1.0 - t) * b012y + t * b123y;

			if (b0123x >= 0 && b0123y >= 0 && b0123x < width && b0123y < height)
			{
				pixels[int(b0123y) * width + int(b0123x)] = rgb(0, 255, 255);
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