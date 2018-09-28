// The Sierpiński triangle. My own algorithm, although someone may have thought of it before.

#include "../boiler/boiler.h"

#include <vector>
#include <iostream>

struct triangle
{
	double ax;
	double ay;

	double bx;
	double by;

	double cx;
	double cy;

	triangle
	(
		double _ax,
		double _ay,

		double _bx,
		double _by,

		double _cx,
		double _cy
	)
	{
		ax = _ax;
		ay = _ay;

		bx = _bx;
		by = _by;

		cx = _cx;
		cy = _cy;
	}

	// Return three triangles to replace this triangle, which results in an iteration of the
	// Sierpiński triangle fractal.

	std::vector<triangle> sierpinski()
	{
		std::vector<triangle> out;

		// Half point mark from ax, ay, to bx, by.

		double habx = ax + (bx - ax) / 2;
		double haby = ay + (by - ay) / 2;

		double hbcx = bx + (cx - bx) / 2;
		double hbcy = by + (cy - by) / 2;

		double hcax = cx + (ax - cx) / 2;
		double hcay = cy + (ay - cy) / 2;

		// Generate triangles.

		out.push_back
		(
			triangle
			(
				ax,
				ay,

				habx,
				haby,

				hcax,
				hcay
			)
		);

		out.push_back
		(
			triangle
			(
				hcax,
				hcay,

				hbcx,
				hbcy,

				cx,
				cy
			)
		);

		out.push_back
		(
			triangle
			(
				habx,
				haby,

				bx,
				by,

				hbcx,
				hbcy
			)
		);

		// Return.

		return out;
	}
};

struct game: boiler
{	
	std::vector<triangle> sierpinski;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "Sierpiński triangle (using Boiler)";

		// Insert the seed triangle. I copied my n-sided polygon generator, because I'm lazy.

		std::vector<double> x;
		std::vector<double> y;

		double theta = degrad(360.0 / 3.0 / 4.0);

		for (int i = 0; i < 3; i++)
		{
			x.push_back(250.0 * cos(2.0 * M_PI * i / 3.0 + theta) + width / 2.0);
			y.push_back(250.0 * sin(2.0 * M_PI * i / 3.0 + theta) + height / 2.0);
		}

		sierpinski.push_back
		(
			triangle
			(
				x[0], y[0] + 250.0 / 4.44,
				x[1], y[1] + 250.0 / 4.44,
				x[2], y[2] + 250.0 / 4.44
			)
		);
	}

	int iteration = 1;

	void keydown(SDL_Event e) override
	{
		iteration++;

		if (e.key.keysym.sym == SDLK_SPACE)
		{
			std::vector<triangle> n_triangles;

			for (int j = 0; j < sierpinski.size(); j++)
			{
				std::vector<triangle> n_iteration = sierpinski[j].sierpinski();

				for (int k = 0; k < n_iteration.size(); k++)
				{
					n_triangles.push_back(n_iteration[k]);
				}
			}

			sierpinski = n_triangles;

			// Log fractal information.

			std::cout << sierpinski.size() << " triangles at iteration " << iteration << "." << std::endl;
		}
	}

	void draw() override
	{
		// Get keypresses.

		const Uint8* keyboard = SDL_GetKeyboardState(NULL);

		memset((void*)pixels, 0, width * height * sizeof(Uint32));

		// Draw the Sierpiński triangle.

		for (int i = 0; i < sierpinski.size(); i++)
		{
			triangle triangle_i = sierpinski[i];

			if (keyboard[SDL_SCANCODE_RETURN] == true)
			{
				ftrianglergb
				(
					triangle_i.ax,
					triangle_i.ay,

					triangle_i.bx,
					triangle_i.by,

					triangle_i.cx,
					triangle_i.cy,

					rgb(255, 255, 255)
				);
			}
			else
			{
				trianglergb
				(
					triangle_i.ax,
					triangle_i.ay,

					triangle_i.bx,
					triangle_i.by,

					triangle_i.cx,
					triangle_i.cy,

					rgb(255, 255, 255)
				);
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