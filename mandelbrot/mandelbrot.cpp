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
	double min_re;
	double max_re;

	double min_im;
	double max_im;

	double factor_re;
	double factor_im;

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
				double c_im = max_im - y * factor_im;

				for (int x = 0; x < width; x++)
				{
					double c_re = min_re + x * factor_re;

					// Calculate whether the complex number defined by c_re and c_im belonds to 
					// the Mandelbrot set.

					double z_re = c_re;
					double z_im = c_im;

					bool z_in = true;

					int n;

					for (n = 0; n < 50; n++)
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

		std::stringstream str;

		double c_re = min_re + mouse_x * factor_re;
		double c_im = max_im - mouse_y * factor_im;

		str << "Real: " << c_re << ", imaginary: " << c_im;

		fontvga(str.str(), 0, 0, 0x7, 0x0);
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