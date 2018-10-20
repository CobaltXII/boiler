// The Mandelbrot fractal? Awesome.

#include "../terminal/terminal_codepage.h"
#include "../terminal/terminal_extras.h"

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>
#include <sstream>

struct game: boiler
{	
	// Complex plane constants.

	double min_re;
	double max_re;

	double min_im;
	double max_im;

	double factor_re;
	double factor_im;

	// Zooming rectangle.

	double zoom_tl_re = 0.0;
	double zoom_tl_im = 0.0;

	double zoom_br_re = 0.0;
	double zoom_br_im = 0.0;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "The Mandelbrot fractal (using Boiler)";

		// Calculate complex plane constants.

		min_re = 0.0 - 2.0;
		max_re = 0.0 + 1.0;

		min_im = 0.0 - 1.2;
		max_im = 0.0 + 1.2;

		factor_re = (max_re - min_re) / (width - 1);
		factor_im = (max_im - min_im) / (height - 1);
	}

	void draw() override
	{
		if (iteration == 0)
		{
			// Render the Mandelbrot.

			clear(rgb(255, 255, 255));	

			for (int y = 0; y < height; y++)
			{
				double c_im = min_im + y * factor_im;

				for (int x = 0; x < width; x++)
				{
					double c_re = min_re + x * factor_re;

					// Calculate whether the complex number defined by c_re and c_im belonds to 
					// the Mandelbrot set.

					double z_re = c_re;
					double z_im = c_im;

					bool z_in = true;

					int n;

					for (n = 0; n < 100; n++)
					{
						double z_re2 = z_re * z_re;
						double z_im2 = z_im * z_im;

						if (z_re2 + z_im2 > 4)
						{
							z_in = false;

							break;
						}

						double z_ims = z_im * z_im;

						z_im = 2 * z_re * z_im + c_im;

						z_re = z_re2 - z_im2 + c_re;
					}

					if (!z_in)
					{
						double g = ((double)n / 50.0);

						pixels[y * width + x] = rgb
						(
							clamprgb(g * 255.0),
							clamprgb(g * 255.0),
							clamprgb(g * 255.0)
						);
					}
					else
					{
						pixels[y * width + x] = rgb(0, 0, 0);
					}
				}
			}
		}

		// Fetch complex number corresponding with mouse pointer.

		double c_re = min_re + mouse_x * factor_re;
		double c_im = min_im + mouse_y * factor_im;

		// Recalculate mouse pointer position from complex number.

		double s_x = (c_re - min_re) / factor_re;
		double s_y = (c_im - min_im) / factor_im;
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