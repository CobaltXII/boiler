// Polar coordinates? Awesome.

#define BOIL_USE_STB_IMAGE

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>

char* path_to_img;

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
	int lena_bw_w;
	int lena_bw_h;

	Uint32* lena_bw;

	// Convert from polar to Cartesian coordinates. X is rho and Y is theta.

	inline point polar_to_cartesian(point polar)
	{
		return point
		(
			polar.x * cos(polar.y),
			polar.x * sin(polar.y)
		);
	}

	// Draw a circle using polar coordinates.

	void polar_circle(double radius, unsigned int color)
	{
		double circ = 2.0 * M_PI * radius;

		double inc = 360.0 / circ;

		// Increase precision.

		circ *= 2.0;

		inc /= 2.0;

		double deg = 0.0;

		for (int i = 0; i < circ; i++)
		{
			point polar_0 = point(radius, degrad(deg));

			point cartesian_0 = polar_to_cartesian(polar_0);

			// Get coordinates from -1.0 to 1.0.

			double norm_x = cartesian_0.x / 288.0;
			double norm_y = cartesian_0.y / 288.0;

			// Normalize to 0.0 to 1.0.

			norm_x = (norm_x + 1.0) / 2.0;
			norm_y = (norm_y + 1.0) / 2.0;

			// Normalize to the width and height of the image.

			norm_x = norm_x * lena_bw_w;
			norm_y = norm_y * lena_bw_h;

			// Use the darkness of the pixel to determine the thickness.

			Uint32 pixel = lena_bw[(int)norm_y * lena_bw_w + (int)norm_x];

			int bw = (mgetr(pixel) + mgetg(pixel) + mgetb(pixel)) / 3;

			double offset = (1.0 - bw / 255.0) * 2.0;

			offset = std::max(offset - 0.1, 0.2);

			point polar_1 = point(radius + offset, degrad(deg));
			point polar_2 = point(radius - offset, degrad(deg));

			point cartesian_1 = polar_to_cartesian(polar_1);
			point cartesian_2 = polar_to_cartesian(polar_2);

			Uint8 cc = 255 - offset * 128;

			linergb
			(
				cartesian_1.x + h_width, cartesian_1.y + h_height,
				cartesian_2.x + h_width, cartesian_2.y + h_height, 

				rgb(cc, cc, cc)
			);

			deg += inc;
		}
	}

	void steam() override
	{
		lena_bw = loadimg(std::string(path_to_img), lena_bw_w, lena_bw_h);

		if (lena_bw == NULL || lena_bw_w < 1 || lena_bw_h < 1)
		{
			nuke("Could not load image.");
		}

		if (lena_bw_w != lena_bw_h)
		{
			std::cout << "You will probably see skewed images, because the image is not square.";
		}

		width = 800;
		height = 600;

		title = "Polar coordinates (using Boiler)";
	}

	void draw() override
	{
		if (iteration == 0)
		{
			clear(rgb(255, 255, 255));

			for (double r = 6.0; r < 6.0 * 48.0; r += 6.0)
			{
				polar_circle(r, rgb(0, 0, 0));
			}
		}
	}
};

// Entry point for the software renderer.

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		nuke("Usage: polar_2 <path_to_img>");
	}

	path_to_img = argv[1];

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