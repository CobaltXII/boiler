/*

The Mandelbrot fractal? Awesome.

Honestly, I had such a great time making this. I encourage everybody who stumbles across this
source code to have a go at making their own; you won't regret it.

I'd like to thank a few people/sources for their help.

	Dirichlet#5635, helped me figure out why my scaling and screen-space to complex-plane-space
	was completely wrong.

	http://warp.povusers.org/Mandelbrot/

		This site helped me a lot with converting the equation of the Mandelbrot set into code.
		It is a very good starting point for creating a Mandelbrot renderer.

	https://solarianprogrammer.com/2013/02/28/mandelbrot-set-cpp-11/

		This site showed me a few ways to calculate color based on slightly modified Bernstein
		polynomials. This helped the renderer look much nicer.

	http://linas.org/art-gallery/escape/escape.html

		This site is pure gold. I love how they don't give formulas and cryptic equations, and
		instead shower you with pseudocode that can be adapted in 5 minutes of work. Without this
		amazing site, my renderer would still be banded by colors.

*/

#include "../boiler/boiler.h"

#include <vector>
#include <utility>
#include <iostream>
#include <sstream>

// Palette kinds.

enum palette_t
{
	pal_red,
	pal_green,
	pal_blue,

	pal_grayscale,
	pal_polynomial
};

palette_t palette = pal_polynomial;

struct game: boiler
{	
	// Maximum iterations.

	double max_iter = 2048.0;

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

	// Smoothing reduces banding.

	bool smoothing = true;

	// Buffer for the Mandelbrot.

	Uint32* mandelbrot_buf;

	void steam() override
	{
		width = 800;
		height = 600;

		title = "The Mandelbrot fractal (using Boiler)";

		// Calculate complex plane constants.

		min_re = 0.0 - 2.0;
		max_re = 0.0 + 1.0;

		min_im = 0.0 - 1.2;

		max_im = min_im + (max_re - min_re) * height / width;

		double min_im_ = 0.0 - (max_im - min_im) / 2.0;
		double max_im_ = 0.0 + (max_im - min_im) / 2.0;

		min_im = min_im_;
		max_im = max_im_;

		factor_re = (max_re - min_re) / (width - 1);
		factor_im = (max_im - min_im) / (height - 1);

		// Allocate a buffer for the Mandelbrot.

		mandelbrot_buf = (Uint32*)malloc(width * height * sizeof(Uint32));
	}

	// Render the Mandelbrot to a seperate buffer.

	void mandelbrot(bool smooth = false)
	{
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

				double n;

				for (n = 0.0; n < max_iter; n++)
				{
					double z_re2 = z_re * z_re;
					double z_im2 = z_im * z_im;

					if (z_re2 + z_im2 > 4.0)
					{
						z_in = false;

						break;
					}

					double z_ims = z_im * z_im;

					z_im = 2.0 * z_re * z_im + c_im;

					z_re = z_re2 - z_im2 + c_re;
				}

				if (!z_in)
				{
					if (smooth)
					{
						// Iterate two more times.

						{
							double z_re2 = z_re * z_re;
							double z_im2 = z_im * z_im;

							double z_ims = z_im * z_im;

							z_im = 2.0 * z_re * z_im + c_im;

							z_re = z_re2 - z_im2 + c_re;

							n++;
						}

						{
							double z_re2 = z_re * z_re;
							double z_im2 = z_im * z_im;

							double z_ims = z_im * z_im;

							z_im = 2.0 * z_re * z_im + c_im;

							z_re = z_re2 - z_im2 + c_re;

							n++;
						}

						double modulus = sqrt(z_re * z_re + z_im * z_im);

						n = n - (log(log(modulus))) / log(2.0);
					}

					n = pow(n / max_iter, 0.5) * max_iter;

					if (palette == pal_red)
					{
						if (n < max_iter / 2 - 1)
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								clamprgb(n / (max_iter / 2.0 - 1.0) * 255.0),

								0.0,
								0.0
							);
						}
						else
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								255.0,

								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0),
								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0)
							);
						}
					}
					else if (palette == pal_green)
					{
						if (n < max_iter / 2 - 1)
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								0.0,

								clamprgb(n / (max_iter / 2.0 - 1.0) * 255.0),

								0.0
							);
						}
						else
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0),

								255.0,

								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0)
							);
						}
					}
					else if (palette == pal_blue)
					{
						if (n < max_iter / 2 - 1)
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								0.0,
								0.0,

								clamprgb(n / (max_iter / 2.0 - 1.0) * 255.0)
							);
						}
						else
						{
							mandelbrot_buf[y * width + x] = rgb
							(
								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0),
								clamprgb((n / max_iter - 0.5) * 2.0 * 255.0),

								255.0
							);
						}
					}
					else if (palette == pal_grayscale)
					{
						double t = n / max_iter;

						mandelbrot_buf[y * width + x] = rgb
						(
							clamprgb(t * 255.0),
							clamprgb(t * 255.0),
							clamprgb(t * 255.0)
						);
					}
					else if (palette == pal_polynomial)
					{
						double t = n / max_iter;

						mandelbrot_buf[y * width + x] = rgb
						(
							clamprgb(9.0 * (1.0 - t) * t * t * t * 255.0),

							clamprgb(15.0 * (1.0 - t) * (1.0 - t) * t * t * 255.0),
							
							clamprgb(8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t * 255.0)
						);
					}
				}
				else
				{
					mandelbrot_buf[y * width + x] = rgb(0, 0, 0);
				}
			}
		}
	}

	void keydown(SDL_Event e) override
	{
		if (e.key.keysym.sym == SDLK_z && !mouse_l)
		{
			// Back to normal.

			min_re = 0.0 - 2.0;
			max_re = 0.0 + 1.0;

			min_im = 0.0 - 1.2;

			max_im = min_im + (max_re - min_re) * height / width;

			factor_re = (max_re - min_re) / (width - 1);
			factor_im = (max_im - min_im) / (height - 1);

			mandelbrot(smoothing);
		}
		else if (e.key.keysym.sym == SDLK_x)
		{
			savebmp("mandelbrot.bmp", pixels, width, height);
		}
		else if (e.key.keysym.sym == SDLK_c)
		{
			// Render again.

			mandelbrot(smoothing);
		}
		else if (e.key.keysym.sym == SDLK_v)
		{
			// Toggle smoothing.

			smoothing = !smoothing;
		}
	}

	void draw() override
	{
		if (iteration == 0)
		{
			// Generate the Mandelbrot.

			mandelbrot(smoothing);
		}

		// Copy the Mandelbrot.

		blitrgb(mandelbrot_buf, 0, 0, width, height, 0, 0, width, height);

		// Fetch complex number corresponding with mouse pointer.

		double c_re = min_re + mouse_x * factor_re;
		double c_im = min_im + mouse_y * factor_im;

		// Recalculate mouse pointer position from complex number.

		double s_x = (c_re - min_re) / factor_re;
		double s_y = (c_im - min_im) / factor_im;

		// Zoom rectangle.

		if (ml_pressed)
		{
			zoom_tl_re = c_re;
			zoom_tl_im = c_im;
		}
		else if (ml_held)
		{
			zoom_br_re = c_re;
			zoom_br_im = c_im;
		}
		else if (ml_released)
		{
			// Apply the rectangle bounds to the screen.

			min_re = zoom_tl_re;
			max_re = zoom_br_re;

			min_im = zoom_tl_im;

			max_im = min_im + (max_re - min_re) * height / width;

			factor_re = (max_re - min_re) / (width - 1);
			factor_im = (max_im - min_im) / (height - 1);

			// Nullify the rectangle bounds.

			zoom_tl_re = 0.0;
			zoom_tl_im = 0.0;

			zoom_br_re = 0.0;
			zoom_br_im = 0.0;

			// Render the Mandelbrot using the new coordinates.

			mandelbrot(true);
		}

		if (ml_held)
		{
			rectrgb
			(
				(zoom_tl_re - min_re) / factor_re,
				(zoom_tl_im - min_im) / factor_im,

				((zoom_br_re - min_re) / factor_re) - ((zoom_tl_re - min_re) / factor_re),
				((zoom_br_im - min_im) / factor_im) - ((zoom_tl_im - min_im) / factor_im),

				rgb(255, 255, 255)
			);
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